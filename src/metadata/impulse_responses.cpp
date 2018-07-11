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

    static const char *ir_source_mono[] =
    {
        "None",
        "Left",
        "Right",
        NULL
    };

    static const char *ir_source_stereo[] =
    {
        "None",
        "File 1 Left",
        "File 1 Right",
        "File 2 Left",
        "File 2 Right",
        NULL
    };

    static const char *ir_fft_rank[] =
    {
        "512",
        "1024",
        "2048",
        "4096",
        "8192",
        "16384",
        "32767",
        "65536",
        NULL
    };

    static const char *ir_file_select[] =
    {
        "File 1",
        "File 2",
        NULL
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

    static const port_t impulse_responses_mono_ports[] =
    {
        // Input audio ports
        PORTS_MONO_PLUGIN,
        IR_COMMON,

        // Input controls
        IR_SAMPLE_FILE("", ""),
        IR_SOURCE("", "", ir_source_mono),

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

        PORTS_END
    };

    static const int impulse_responses_classes[] = { C_CONVERTER, C_SPECTRAL, -1 };

    const plugin_metadata_t  impulse_responses_mono_metadata::metadata =
    {
        "Impulsantworten Mono",
        "Impulse Responses Mono",
        "IA1M",
        &developers::v_sadovnikov,
        "impulse_responses_mono",
        "wvwt",
        LSP_IMPULSE_RESPONSES_BASE + 0,
        LSP_VERSION(1, 0, 1),
        impulse_responses_classes,
        impulse_responses_mono_ports,
        "convolution/impulse_responses/mono.xml",
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
        LSP_IMPULSE_RESPONSES_BASE + 1,
        LSP_VERSION(1, 0, 1),
        impulse_responses_classes,
        impulse_responses_stereo_ports,
        "convolution/impulse_responses/stereo.xml",
        stereo_plugin_port_groups
    };
}



