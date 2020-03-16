/*
 * experimental.cpp
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
    // Impulse responses

    static const port_item_t ir_source_mono[] =
    {
        { "None",       "file.none" },
        { "Left",       "file.left" },
        { "Right",      "file.right" },
        { NULL, NULL }
    };

    static const port_item_t ir_source_stereo[] =
    {
        { "None",           "file.none" },
        { "File 1 Left",    "file.f1l" },
        { "File 1 Right",   "file.f1r" },
        { "File 2 Left",    "file.f2l" },
        { "File 2 Right",   "file.f2r" },
        { NULL, NULL }
    };

    static const port_item_t ir_fft_rank[] =
    {
        { "512",    NULL },
        { "1024",   NULL },
        { "2048",   NULL },
        { "4096",   NULL },
        { "8192",   NULL },
        { "16384",  NULL },
        { "32767",  NULL },
        { "65536",  NULL },
        { NULL, NULL }
    };

    static const port_item_t ir_file_select[] =
    {
        { "File 1",     "file.f1" },
        { "File 2",     "file.f2" },
        { NULL, NULL }
    };

    static const port_item_t filter_slope[] =
    {
        { "off",        "eq.slope.off" },
        { "6 dB/oct",   "eq.slope.6dbo" },
        { "12 dB/oct",  "eq.slope.12dbo" },
        { "18 dB/oct",  "eq.slope.18dbo" },
        { NULL, NULL }
    };

    #define IR_COMMON \
        BYPASS, \
        COMBO("fft", "FFT size", impulse_responses_base_metadata::FFT_RANK_DEFAULT, ir_fft_rank), \
        DRY_GAIN(1.0f), \
        WET_GAIN(1.0f), \
        OUT_GAIN

    #define IR_SAMPLE_FILE(id, label)   \
        PATH("ifn" id, "Impulse file" label),    \
        CONTROL("ihc" id, "Head cut" label, U_MSEC, impulse_responses_base_metadata::CONV_LENGTH), \
        CONTROL("itc" id, "Tail cut" label, U_MSEC, impulse_responses_base_metadata::CONV_LENGTH), \
        CONTROL("ifi" id, "Fade in" label, U_MSEC, impulse_responses_base_metadata::CONV_LENGTH), \
        CONTROL("ifo" id, "Fade out" label, U_MSEC, impulse_responses_base_metadata::CONV_LENGTH), \
        TRIGGER("ils" id, "Impulse listen" label), \
        STATUS("ifs" id, "Load status" label), \
        METER("ifl" id, "Impulse length" label, U_MSEC, impulse_responses_base_metadata::CONV_LENGTH), \
        MESH("ifd" id, "Impulse file contents" label, impulse_responses_base_metadata::TRACKS_MAX, impulse_responses_base_metadata::MESH_SIZE)

    #define IR_SOURCE(id, label, select) \
        COMBO("cs" id, "Channel source" label, 0, select), \
        AMP_GAIN100("mk" id, "Makeup gain" label, 1.0f), \
        BLINK("ca" id, "Channel activity" label), \
        CONTROL("pd" id, "Pre-delay" label, U_MSEC, impulse_responses_base_metadata::PREDELAY)

    #define IR_EQ_BAND(id, freq)    \
        CONTROL("eq_" #id, "Band " freq "Hz gain", U_GAIN_AMP, impulse_responses_base_metadata::BA)

    #define IR_EQUALIZER    \
        SWITCH("wpp", "Wet post-process", 0),    \
        COMBO("lcm", "Low-cut mode", 0, filter_slope),      \
        CONTROL("lcf", "Low-cut frequency", U_HZ, impulse_responses_base_metadata::LCF),   \
        IR_EQ_BAND(0, "50"), \
        IR_EQ_BAND(1, "107"), \
        IR_EQ_BAND(2, "227"), \
        IR_EQ_BAND(3, "484"), \
        IR_EQ_BAND(4, "1 k"), \
        IR_EQ_BAND(5, "2.2 k"), \
        IR_EQ_BAND(6, "4.7 k"), \
        IR_EQ_BAND(7, "10 k"), \
        COMBO("hcm", "High-cut mode", 0, filter_slope),      \
        CONTROL("hcf", "High-cut frequency", U_HZ, impulse_responses_base_metadata::HCF)

    static const port_t impulse_responses_mono_ports[] =
    {
        // Input audio ports
        PORTS_MONO_PLUGIN,
        IR_COMMON,

        // Input controls
        IR_SAMPLE_FILE("", ""),
        IR_SOURCE("", "", ir_source_mono),
        IR_EQUALIZER,

        PORTS_END
    };

    static const port_t impulse_responses_stereo_ports[] =
    {
        // Input audio ports
        PORTS_STEREO_PLUGIN,
        IR_COMMON,
        COMBO("fsel", "File selector", 0, ir_file_select), \

        // Input controls
        IR_SAMPLE_FILE("0", " 1"),
        IR_SAMPLE_FILE("1", " 2"),
        IR_SOURCE("_l", " Left", ir_source_stereo),
        IR_SOURCE("_r", " Right", ir_source_stereo),
        IR_EQUALIZER,

        PORTS_END
    };

    static const int impulse_responses_classes[] = { C_REVERB, -1 };

    const plugin_metadata_t  impulse_responses_mono_metadata::metadata =
    {
        "Impulsantworten Mono",
        "Impulse Responses Mono",
        "IA1M",
        &developers::v_sadovnikov,
        "impulse_responses_mono",
        "wvwt",
        0,
        LSP_VERSION(1, 0, 2),
        impulse_responses_classes,
        E_NONE,
        impulse_responses_mono_ports,
        "convolution/impulse_responses/mono.xml",
        NULL,
        mono_plugin_port_groups
    };

    const plugin_metadata_t  impulse_responses_stereo_metadata::metadata =
    {
        "Impulsantworten Stereo",
        "Impulse Responses Stereo",
        "IA1S",
        &developers::v_sadovnikov,
        "impulse_responses_stereo",
        "1khz",
        0,
        LSP_VERSION(1, 0, 2),
        impulse_responses_classes,
        E_NONE,
        impulse_responses_stereo_ports,
        "convolution/impulse_responses/stereo.xml",
        NULL,
        stereo_plugin_port_groups
    };
}



