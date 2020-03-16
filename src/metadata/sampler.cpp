/*
 * sampler.cpp
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
    // Sampler
    static const port_item_t sampler_sample_selectors[] =
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

    static const port_item_t mute_groups[] =
    {
        { "None", "sampler.samp.none" },
        { "A", NULL },
        { "B", NULL },
        { "C", NULL },
        { "D", NULL },
        { "E", NULL },
        { "F", NULL },
        { "G", NULL },
        { "H", NULL },
        { "I", NULL },
        { "J", NULL },
        { "K", NULL },
        { "L", NULL },
        { "M", NULL },
        { "N", NULL },
        { "O", NULL },
        { "P", NULL },
        { "Q", NULL },
        { "R", NULL },
        { "S", NULL },
        { "T", NULL },
        { "U", NULL },
        { "V", NULL },
        { "W", NULL },
        { "X", NULL },
        { "Y", NULL },
        { "Z", NULL },
        { NULL, NULL }
    };

    #define R(x) { x, "sampler.inst." x }
    static const port_item_t sampler_x12_instruments[] =
    {
        R("0"), R("1"), R("2"), R("3"),
        R("4"), R("5"), R("6"), R("7"),
        R("8"), R("9"), R("10"), R("11"),
        { NULL, NULL }
    };

    static const port_item_t sampler_x24_instruments[] =
    {
        R("0"), R("1"), R("2"), R("3"),
        R("4"), R("5"), R("6"), R("7"),
        R("8"), R("9"), R("10"), R("11"),
        R("12"), R("13"), R("14"), R("15"),
        R("16"), R("17"), R("18"), R("19"),
        R("20"), R("21"), R("22"), R("23"),
        { NULL, NULL }
    };

    static const port_item_t sampler_x48_instruments[] =
    {
        R("0"), R("1"), R("2"), R("3"),
        R("4"), R("5"), R("6"), R("7"),
        R("8"), R("9"), R("10"), R("11"),
        R("12"), R("13"), R("14"), R("15"),
        R("16"), R("17"), R("18"), R("19"),
        R("20"), R("21"), R("22"), R("23"),
        R("24"), R("25"), R("26"), R("27"),
        R("28"), R("29"), R("30"), R("31"),
        R("32"), R("33"), R("34"), R("35"),
        R("36"), R("37"), R("38"), R("39"),
        R("40"), R("41"), R("42"), R("43"),
        R("44"), R("45"), R("46"), R("47"),
        { NULL, NULL }
    };
    #undef R

    static const port_item_t sampler_x12_mixer_lines[] =
    {
        { "Instruments", "sampler.instruments" },
        { "Mixer", "sampler.mixer" },
        { NULL, NULL }
    };

    static const port_item_t sampler_x24_mixer_lines[] =
    {
        { "Instruments", "sampler.instruments" },
        { "Mixer 0-11", "sampler.mixer_0:11" },
        { "Mixer 12-23", "sampler.mixer_12:23" },
        { NULL, NULL }
    };

    static const port_item_t sampler_x48_mixer_lines[] =
    {
        { "Instruments", "sampler.instruments" },
        { "Mixer 0-11", "sampler.mixer_0:11" },
        { "Mixer 12-23", "sampler.mixer_12:23" },
        { "Mixer 24-35", "sampler.mixer_24:35" },
        { "Mixer 36-47", "sampler.mixer_36:47" },
        { NULL, NULL }
    };

    #define S_DO_GROUP_PORTS(i) \
        STEREO_PORT_GROUP_PORTS(dout_ ## i, "dol_" #i, "dor_" #i)

    #define S_DO_GROUP(i) \
        { "direct_out_" #i, "Direct Output " #i,    GRP_STEREO,     PGF_OUT,    dout_ ## i ##_ports      }

    #define S_FILE_GAIN_MONO \
        AMP_GAIN10("mx", "Sample mix gain", 1.0f)
    #define S_FILE_GAIN_STEREO \
        PAN_CTL("pl", "Sample left channel panorama", -100.0f), \
        PAN_CTL("pr", "Sample right channel panorama", 100.0f)

    #define S_PORTS_GLOBAL      \
        BYPASS,                 \
        TRIGGER("mute", "Forced mute"), \
        SWITCH("muting", "Mute on stop", 1.0f), \
        SWITCH("noff", "Note-off handling", 0.0f), \
        CONTROL("fout", "Note-off fadeout", U_MSEC, sampler_kernel_metadata::FADEOUT), \
        DRY_GAIN(1.0f),         \
        WET_GAIN(1.0f),         \
        OUT_GAIN

    #define S_DO_CONTROL \
        SWITCH("do_gain", "Apply gain to direct-out", 1.0f), \
        SWITCH("do_pan", "Apply panning to direct-out", 1.0f)

    #define S_SAMPLE_FILE(gain)        \
        PATH("sf", "Sample file"), \
        CONTROL("hc", "Sample head cut", U_MSEC, sampler_kernel_metadata::SAMPLE_LENGTH), \
    	CONTROL("tc", "Sample tail cut", U_MSEC, sampler_kernel_metadata::SAMPLE_LENGTH), \
        CONTROL("fi", "Sample fade in", U_MSEC, sampler_kernel_metadata::SAMPLE_LENGTH), \
        CONTROL("fo", "Sample fade out", U_MSEC, sampler_kernel_metadata::SAMPLE_LENGTH), \
        AMP_GAIN10("mk", "Sample makeup gain", 1.0f), \
        { "vl", "Sample velocity max",  U_PERCENT, R_CONTROL, F_IN | F_LOWER | F_UPPER | F_STEP | F_LOWERING, 0.0f, 100.0f, 0.0f, 0.05, NULL }, \
        { "pd", "Sample pre-delay",  U_MSEC, R_CONTROL, F_IN | F_LOWER | F_UPPER | F_STEP, \
                sampler_kernel_metadata::PREDELAY_MIN, sampler_kernel_metadata::PREDELAY_MAX, 0, sampler_kernel_metadata::PREDELAY_STEP, NULL }, \
        SWITCH("on", "Sample enabled", 1.0f), \
        TRIGGER("ls", "Sample listen"), \
        gain, \
        BLINK("ac", "Sample activity"), \
        BLINK("no", "Sample note on event"), \
        { "fl", "Sample length", U_MSEC, R_METER, F_OUT | F_LOWER | F_UPPER | F_STEP, \
                sampler_kernel_metadata::SAMPLE_LENGTH_MIN, sampler_kernel_metadata::SAMPLE_LENGTH_MAX, 0, sampler_kernel_metadata::SAMPLE_LENGTH_STEP, NULL }, \
        STATUS("fs", "Sample load status"), \
        MESH("fd", "Sample file contents", sampler_kernel_metadata::TRACKS_MAX, sampler_kernel_metadata::MESH_SIZE)

    #define S_INSTRUMENT(sample)    \
        COMBO("chan", "Channel", sampler_kernel_metadata::CHANNEL_DFL, midi_channels), \
        COMBO("note", "Note", sampler_kernel_metadata::NOTE_DFL, notes), \
        COMBO("oct", "Octave", sampler_kernel_metadata::OCTAVE_DFL, octaves), \
        { "mn", "MIDI Note #", U_NONE, R_METER, F_OUT | F_LOWER | F_UPPER | F_INT, 0, 127, 0, 0, NULL }, \
        TRIGGER("trg", "Instrument listen"), \
        CONTROL("dyna", "Dynamics", U_PERCENT, sampler_base_metadata::DYNA), \
        CONTROL("drft", "Time drifting", U_MSEC, sampler_base_metadata::DRIFT), \
        PORT_SET("ssel", "Sample selector", sampler_sample_selectors, sample)

    #define S_MG_INSTRUMENT(sample)    \
        COMBO("chan", "Channel", sampler_kernel_metadata::CHANNEL_DFL, midi_channels), \
        COMBO("note", "Note", sampler_kernel_metadata::NOTE_DFL, notes), \
        COMBO("oct", "Octave", sampler_kernel_metadata::OCTAVE_DFL, octaves), \
        COMBO("mgrp", "Mute Group", 0, mute_groups), \
        SWITCH("mtg", "Mute on stop", 0.0f), \
        SWITCH("nto", "Note-off handling", 0.0f), \
        { "mn", "MIDI Note #", U_NONE, R_METER, F_OUT | F_LOWER | F_UPPER | F_INT, 0, 127, 0, 0, NULL }, \
        TRIGGER("trg", "Instrument listen"), \
        CONTROL("dyna", "Dynamics", U_PERCENT, sampler_base_metadata::DYNA), \
        CONTROL("drft", "Time drifting", U_MSEC, sampler_base_metadata::DRIFT), \
        PORT_SET("ssel", "Sample selector", sampler_sample_selectors, sample)

    #define S_AREA_SELECTOR(list)     \
        COMBO("msel", "Area selector", 0, list)

    #define S_INSTRUMENT_SELECTOR(list)     \
        PORT_SET("inst", "Instrument selector", list, sampler_multiple_ports)

    #define S_MIXER(id)                      \
        SWITCH("ion_" #id, "Instrument on " #id, 1.0f), \
        AMP_GAIN10("imix_" #id, "Instrument mix gain " #id, 1.0f), \
        PAN_CTL("panl_" #id, "Instrument panorama left " #id, -100.0f), \
        PAN_CTL("panr_" #id, "Instrument manorama right " #id, 100.0f), \
        BLINK("iact_" #id, "Instrument activity " #id)

    #define S_DIRECT_OUT(id)                \
        S_MIXER(id),                        \
        SWITCH("don_" #id, "Direct output on " #id, 1.0f), \
        AUDIO_OUTPUT("dol_" #id, "Direct output left " #id), \
        AUDIO_OUTPUT("dor_" #id, "Direct output right " #id)

    // Define stereo direct outputs
    S_DO_GROUP_PORTS(0);
    S_DO_GROUP_PORTS(1);
    S_DO_GROUP_PORTS(2);
    S_DO_GROUP_PORTS(3);
    S_DO_GROUP_PORTS(4);
    S_DO_GROUP_PORTS(5);
    S_DO_GROUP_PORTS(6);
    S_DO_GROUP_PORTS(7);
    S_DO_GROUP_PORTS(8);
    S_DO_GROUP_PORTS(9);
    S_DO_GROUP_PORTS(10);
    S_DO_GROUP_PORTS(11);
    S_DO_GROUP_PORTS(12);
    S_DO_GROUP_PORTS(13);
    S_DO_GROUP_PORTS(14);
    S_DO_GROUP_PORTS(15);
    S_DO_GROUP_PORTS(16);
    S_DO_GROUP_PORTS(17);
    S_DO_GROUP_PORTS(18);
    S_DO_GROUP_PORTS(19);
    S_DO_GROUP_PORTS(20);
    S_DO_GROUP_PORTS(21);
    S_DO_GROUP_PORTS(22);
    S_DO_GROUP_PORTS(23);
    S_DO_GROUP_PORTS(24);
    S_DO_GROUP_PORTS(25);
    S_DO_GROUP_PORTS(26);
    S_DO_GROUP_PORTS(27);
    S_DO_GROUP_PORTS(28);
    S_DO_GROUP_PORTS(29);
    S_DO_GROUP_PORTS(30);
    S_DO_GROUP_PORTS(31);
    S_DO_GROUP_PORTS(32);
    S_DO_GROUP_PORTS(33);
    S_DO_GROUP_PORTS(34);
    S_DO_GROUP_PORTS(35);
    S_DO_GROUP_PORTS(36);
    S_DO_GROUP_PORTS(37);
    S_DO_GROUP_PORTS(38);
    S_DO_GROUP_PORTS(39);
    S_DO_GROUP_PORTS(40);
    S_DO_GROUP_PORTS(41);
    S_DO_GROUP_PORTS(42);
    S_DO_GROUP_PORTS(43);
    S_DO_GROUP_PORTS(44);
    S_DO_GROUP_PORTS(45);
    S_DO_GROUP_PORTS(46);
    S_DO_GROUP_PORTS(47);

    // Define Direct-output port groups
    const port_group_t sampler_x12_port_groups[] =
    {
        { "stereo_in",  "Stereo Input",     GRP_STEREO,     PGF_IN,     stereo_in_group_ports       },
        { "stereo_out", "Stereo Output",    GRP_STEREO,     PGF_OUT,    stereo_out_group_ports      },
        S_DO_GROUP(0),
        S_DO_GROUP(1),
        S_DO_GROUP(2),
        S_DO_GROUP(3),
        S_DO_GROUP(4),
        S_DO_GROUP(5),
        S_DO_GROUP(6),
        S_DO_GROUP(7),
        S_DO_GROUP(8),
        S_DO_GROUP(9),
        S_DO_GROUP(10),
        S_DO_GROUP(11),
        { NULL, NULL }
    };

    const port_group_t sampler_x24_port_groups[] =
    {
        { "stereo_in",  "Stereo Input",     GRP_STEREO,     PGF_IN,     stereo_in_group_ports       },
        { "stereo_out", "Stereo Output",    GRP_STEREO,     PGF_OUT,    stereo_out_group_ports      },
        S_DO_GROUP(0),
        S_DO_GROUP(1),
        S_DO_GROUP(2),
        S_DO_GROUP(3),
        S_DO_GROUP(4),
        S_DO_GROUP(5),
        S_DO_GROUP(6),
        S_DO_GROUP(7),
        S_DO_GROUP(8),
        S_DO_GROUP(9),
        S_DO_GROUP(10),
        S_DO_GROUP(11),
        S_DO_GROUP(12),
        S_DO_GROUP(13),
        S_DO_GROUP(14),
        S_DO_GROUP(15),
        S_DO_GROUP(16),
        S_DO_GROUP(17),
        S_DO_GROUP(18),
        S_DO_GROUP(19),
        S_DO_GROUP(20),
        S_DO_GROUP(21),
        S_DO_GROUP(22),
        S_DO_GROUP(23),
        { NULL, NULL }
    };

    const port_group_t sampler_x48_port_groups[] =
    {
        { "stereo_in",  "Stereo Input",     GRP_STEREO,     PGF_IN,     stereo_in_group_ports       },
        { "stereo_out", "Stereo Output",    GRP_STEREO,     PGF_OUT,    stereo_out_group_ports      },
        S_DO_GROUP(0),
        S_DO_GROUP(1),
        S_DO_GROUP(2),
        S_DO_GROUP(3),
        S_DO_GROUP(4),
        S_DO_GROUP(5),
        S_DO_GROUP(6),
        S_DO_GROUP(7),
        S_DO_GROUP(8),
        S_DO_GROUP(9),
        S_DO_GROUP(10),
        S_DO_GROUP(11),
        S_DO_GROUP(12),
        S_DO_GROUP(13),
        S_DO_GROUP(14),
        S_DO_GROUP(15),
        S_DO_GROUP(16),
        S_DO_GROUP(17),
        S_DO_GROUP(18),
        S_DO_GROUP(19),
        S_DO_GROUP(20),
        S_DO_GROUP(21),
        S_DO_GROUP(22),
        S_DO_GROUP(23),
        S_DO_GROUP(24),
        S_DO_GROUP(25),
        S_DO_GROUP(26),
        S_DO_GROUP(27),
        S_DO_GROUP(28),
        S_DO_GROUP(29),
        S_DO_GROUP(30),
        S_DO_GROUP(31),
        S_DO_GROUP(32),
        S_DO_GROUP(33),
        S_DO_GROUP(34),
        S_DO_GROUP(35),
        S_DO_GROUP(36),
        S_DO_GROUP(37),
        S_DO_GROUP(38),
        S_DO_GROUP(39),
        S_DO_GROUP(40),
        S_DO_GROUP(41),
        S_DO_GROUP(42),
        S_DO_GROUP(43),
        S_DO_GROUP(44),
        S_DO_GROUP(45),
        S_DO_GROUP(46),
        S_DO_GROUP(47),
        { NULL, NULL }
    };

    // Define port sets
    static const port_t sample_file_mono_ports[] =
    {
        S_SAMPLE_FILE(S_FILE_GAIN_MONO),
        PORTS_END
    };

    static const port_t sample_file_stereo_ports[] =
    {
        S_SAMPLE_FILE(S_FILE_GAIN_STEREO),
        PORTS_END
    };

    static const port_t sampler_multiple_ports[] =
    {
        S_MG_INSTRUMENT(sample_file_stereo_ports),
        PORTS_END
    };

    static const int sampler_classes[] = { C_INSTRUMENT, -1 };

    // Define port lists for each plugin
    static const port_t sampler_mono_ports[] =
    {
        PORTS_MONO_PLUGIN,
        PORTS_MIDI_CHANNEL,
        S_PORTS_GLOBAL,
        S_INSTRUMENT(sample_file_mono_ports),

        PORTS_END
    };

    static const port_t sampler_stereo_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        PORTS_MIDI_CHANNEL,
        S_PORTS_GLOBAL,
        S_INSTRUMENT(sample_file_stereo_ports),

        PORTS_END
    };

    static const port_t sampler_x12_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        PORTS_MIDI_CHANNEL,
        S_PORTS_GLOBAL,
        S_AREA_SELECTOR(sampler_x12_mixer_lines),
        S_INSTRUMENT_SELECTOR(sampler_x12_instruments),
        S_MIXER(0),
        S_MIXER(1),
        S_MIXER(2),
        S_MIXER(3),
        S_MIXER(4),
        S_MIXER(5),
        S_MIXER(6),
        S_MIXER(7),
        S_MIXER(8),
        S_MIXER(9),
        S_MIXER(10),
        S_MIXER(11),

        PORTS_END
    };

    static const port_t sampler_x24_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        PORTS_MIDI_CHANNEL,
        S_PORTS_GLOBAL,
        S_AREA_SELECTOR(sampler_x24_mixer_lines),
        S_INSTRUMENT_SELECTOR(sampler_x24_instruments),
        S_MIXER(0),
        S_MIXER(1),
        S_MIXER(2),
        S_MIXER(3),
        S_MIXER(4),
        S_MIXER(5),
        S_MIXER(6),
        S_MIXER(7),
        S_MIXER(8),
        S_MIXER(9),
        S_MIXER(10),
        S_MIXER(11),
        S_MIXER(12),
        S_MIXER(13),
        S_MIXER(14),
        S_MIXER(15),
        S_MIXER(16),
        S_MIXER(17),
        S_MIXER(18),
        S_MIXER(19),
        S_MIXER(20),
        S_MIXER(21),
        S_MIXER(22),
        S_MIXER(23),

        PORTS_END
    };

    static const port_t sampler_x48_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        PORTS_MIDI_CHANNEL,
        S_PORTS_GLOBAL,
        S_AREA_SELECTOR(sampler_x48_mixer_lines),
        S_INSTRUMENT_SELECTOR(sampler_x48_instruments),
        S_MIXER(0),
        S_MIXER(1),
        S_MIXER(2),
        S_MIXER(3),
        S_MIXER(4),
        S_MIXER(5),
        S_MIXER(6),
        S_MIXER(7),
        S_MIXER(8),
        S_MIXER(9),
        S_MIXER(10),
        S_MIXER(11),
        S_MIXER(12),
        S_MIXER(13),
        S_MIXER(14),
        S_MIXER(15),
        S_MIXER(16),
        S_MIXER(17),
        S_MIXER(18),
        S_MIXER(19),
        S_MIXER(20),
        S_MIXER(21),
        S_MIXER(22),
        S_MIXER(23),
        S_MIXER(24),
        S_MIXER(25),
        S_MIXER(26),
        S_MIXER(27),
        S_MIXER(28),
        S_MIXER(29),
        S_MIXER(30),
        S_MIXER(31),
        S_MIXER(32),
        S_MIXER(33),
        S_MIXER(34),
        S_MIXER(35),
        S_MIXER(36),
        S_MIXER(37),
        S_MIXER(38),
        S_MIXER(39),
        S_MIXER(40),
        S_MIXER(41),
        S_MIXER(42),
        S_MIXER(43),
        S_MIXER(44),
        S_MIXER(45),
        S_MIXER(46),
        S_MIXER(47),

        PORTS_END
    };

    static const port_t sampler_x12_do_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        PORTS_MIDI_CHANNEL,
        S_PORTS_GLOBAL,
        S_DO_CONTROL,
        S_AREA_SELECTOR(sampler_x12_mixer_lines),
        S_INSTRUMENT_SELECTOR(sampler_x12_instruments),
        S_DIRECT_OUT(0),
        S_DIRECT_OUT(1),
        S_DIRECT_OUT(2),
        S_DIRECT_OUT(3),
        S_DIRECT_OUT(4),
        S_DIRECT_OUT(5),
        S_DIRECT_OUT(6),
        S_DIRECT_OUT(7),
        S_DIRECT_OUT(8),
        S_DIRECT_OUT(9),
        S_DIRECT_OUT(10),
        S_DIRECT_OUT(11),

        PORTS_END
    };

    static const port_t sampler_x24_do_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        PORTS_MIDI_CHANNEL,
        S_PORTS_GLOBAL,
        S_DO_CONTROL,
        S_AREA_SELECTOR(sampler_x24_mixer_lines),
        S_INSTRUMENT_SELECTOR(sampler_x24_instruments),
        S_DIRECT_OUT(0),
        S_DIRECT_OUT(1),
        S_DIRECT_OUT(2),
        S_DIRECT_OUT(3),
        S_DIRECT_OUT(4),
        S_DIRECT_OUT(5),
        S_DIRECT_OUT(6),
        S_DIRECT_OUT(7),
        S_DIRECT_OUT(8),
        S_DIRECT_OUT(9),
        S_DIRECT_OUT(10),
        S_DIRECT_OUT(11),
        S_DIRECT_OUT(12),
        S_DIRECT_OUT(13),
        S_DIRECT_OUT(14),
        S_DIRECT_OUT(15),
        S_DIRECT_OUT(16),
        S_DIRECT_OUT(17),
        S_DIRECT_OUT(18),
        S_DIRECT_OUT(19),
        S_DIRECT_OUT(20),
        S_DIRECT_OUT(21),
        S_DIRECT_OUT(22),
        S_DIRECT_OUT(23),

        PORTS_END
    };

    static const port_t sampler_x48_do_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        PORTS_MIDI_CHANNEL,
        S_PORTS_GLOBAL,
        S_DO_CONTROL,
        S_AREA_SELECTOR(sampler_x48_mixer_lines),
        S_INSTRUMENT_SELECTOR(sampler_x48_instruments),
        S_DIRECT_OUT(0),
        S_DIRECT_OUT(1),
        S_DIRECT_OUT(2),
        S_DIRECT_OUT(3),
        S_DIRECT_OUT(4),
        S_DIRECT_OUT(5),
        S_DIRECT_OUT(6),
        S_DIRECT_OUT(7),
        S_DIRECT_OUT(8),
        S_DIRECT_OUT(9),
        S_DIRECT_OUT(10),
        S_DIRECT_OUT(11),
        S_DIRECT_OUT(12),
        S_DIRECT_OUT(13),
        S_DIRECT_OUT(14),
        S_DIRECT_OUT(15),
        S_DIRECT_OUT(16),
        S_DIRECT_OUT(17),
        S_DIRECT_OUT(18),
        S_DIRECT_OUT(19),
        S_DIRECT_OUT(20),
        S_DIRECT_OUT(21),
        S_DIRECT_OUT(22),
        S_DIRECT_OUT(23),
        S_DIRECT_OUT(24),
        S_DIRECT_OUT(25),
        S_DIRECT_OUT(26),
        S_DIRECT_OUT(27),
        S_DIRECT_OUT(28),
        S_DIRECT_OUT(29),
        S_DIRECT_OUT(30),
        S_DIRECT_OUT(31),
        S_DIRECT_OUT(32),
        S_DIRECT_OUT(33),
        S_DIRECT_OUT(34),
        S_DIRECT_OUT(35),
        S_DIRECT_OUT(36),
        S_DIRECT_OUT(37),
        S_DIRECT_OUT(38),
        S_DIRECT_OUT(39),
        S_DIRECT_OUT(40),
        S_DIRECT_OUT(41),
        S_DIRECT_OUT(42),
        S_DIRECT_OUT(43),
        S_DIRECT_OUT(44),
        S_DIRECT_OUT(45),
        S_DIRECT_OUT(46),
        S_DIRECT_OUT(47),

        PORTS_END
    };

    //-------------------------------------------------------------------------
    // Define plugin metadata
    const plugin_metadata_t  sampler_mono_metadata::metadata =
    {
        "Klangerzeuger Mono",
        "Sampler Mono",
        "KZ1M",
        &developers::v_sadovnikov,
        "sampler_mono",
        "ca4r",
        0,
        LSP_VERSION(1, 0, 2),
        sampler_classes,
        E_NONE,
        sampler_mono_ports,
        "sampling/single/mono.xml",
        NULL,
        mono_plugin_port_groups
    };

    const plugin_metadata_t  sampler_stereo_metadata::metadata =
    {
        "Klangerzeuger Stereo",
        "Sampler Stereo",
        "KZ1S",
        &developers::v_sadovnikov,
        "sampler_stereo",
        "kjw3",
        0,
        LSP_VERSION(1, 0, 2),
        sampler_classes,
        E_NONE,
        sampler_stereo_ports,
        "sampling/single/stereo.xml",
        NULL,
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  multisampler_x12_metadata::metadata =
    {
        "Schlagzeug x12 Stereo",
        "Multi-Sampler x12 Stereo",
        "SZ12",
        &developers::v_sadovnikov,
        "multisampler_x12",
        "clrs",
        0,
        LSP_VERSION(1, 0, 2),
        sampler_classes,
        E_NONE,
        sampler_x12_ports,
        "sampling/multiple/x12.xml",
        NULL,
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  multisampler_x24_metadata::metadata =
    {
        "Schlagzeug x24 Stereo",
        "Multi-Sampler x24 Stereo",
        "SZ24",
        &developers::v_sadovnikov,
        "multisampler_x24",
        "visl",
        0,
        LSP_VERSION(1, 0, 2),
        sampler_classes,
        E_NONE,
        sampler_x24_ports,
        "sampling/multiple/x24.xml",
        NULL,
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  multisampler_x48_metadata::metadata =
    {
        "Schlagzeug x48 Stereo",
        "Multi-Sampler x48 Stereo",
        "SZ48",
        &developers::v_sadovnikov,
        "multisampler_x48",
        "hnj4",
        0,
        LSP_VERSION(1, 0, 2),
        sampler_classes,
        E_NONE,
        sampler_x48_ports,
        "sampling/multiple/x48.xml",
        NULL,
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  multisampler_x12_do_metadata::metadata =
    {
        "Schlagzeug x12 Direktausgabe",
        "Multi-Sampler x12 DirectOut",
        "SZ12D",
        &developers::v_sadovnikov,
        "multisampler_x12_do",
        "7zkj",
        0,
        LSP_VERSION(1, 0, 2),
        sampler_classes,
        E_NONE,
        sampler_x12_do_ports,
        "sampling/multiple/x12_do.xml",
        NULL,
        sampler_x12_port_groups
    };

    const plugin_metadata_t  multisampler_x24_do_metadata::metadata =
    {
        "Schlagzeug x24 Direktausgabe",
        "Multi-Sampler x24 DirectOut",
        "SZ24D",
        &developers::v_sadovnikov,
        "multisampler_x24_do",
        "vimj",
        0,
        LSP_VERSION(1, 0, 2),
        sampler_classes,
        E_NONE,
        sampler_x24_do_ports,
        "sampling/multiple/x24_do.xml",
        NULL,
        sampler_x24_port_groups
    };

    const plugin_metadata_t  multisampler_x48_do_metadata::metadata =
    {
        "Schlagzeug x48 Direktausgabe",
        "Multi-Sampler x48 DirectOut",
        "SZ48D",
        &developers::v_sadovnikov,
        "multisampler_x48_do",
        "blyi",
        0,
        LSP_VERSION(1, 0, 2),
        sampler_classes,
        E_NONE,
        sampler_x48_do_ports,
        "sampling/multiple/x48_do.xml",
        NULL,
        sampler_x48_port_groups
    };

}



