/*
 * impulse_reverb.cpp
 *
*  Created on: 13 фев. 2017 г.
 *      Author: sadko
 */

#include <metadata/plugins.h>
#include <metadata/ports.h>
#include <metadata/developers.h>

namespace lsp
{
    //-------------------------------------------------------------------------
    // Impulse reverb

    static const char *ir_source[] =
    {
        "None",
        "File 1 Left",
        "File 1 Right",
        "File 2 Left",
        "File 2 Right",
        "File 3 Left",
        "File 3 Right",
        "File 4 Left",
        "File 4 Right",

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
        "File 3",
        "File 4",
        NULL
    };

    #define IR_PAN_MONO \
        PAN_CTL("p", "Panorama", 0.0f)

    #define IR_PAN_STEREO \
        PAN_CTL("pl", "Left channel panorama", -100.0f), \
        PAN_CTL("pr", "Right channel panorama", 100.0f)

    #define IR_COMMON(pan) \
        BYPASS, \
        COMBO("fsel", "File selector", 0, ir_file_select), \
        COMBO("fft", "FFT size", impulse_reverb_base_metadata::FFT_RANK_DEFAULT, ir_fft_rank), \
        CONTROL("pd", "Pre-delay", U_MSEC, impulse_reverb_base_metadata::PREDELAY), \
        pan, \
        DRY_GAIN(1.0f), \
        WET_GAIN(1.0f), \
        OUT_GAIN

    #define IR_SAMPLE_FILE(id, label)   \
        PATH("ifn" id, "Impulse file" label),    \
        CONTROL("ihc" id, "Head cut" label, U_MSEC, impulse_reverb_base_metadata::CONV_LENGTH), \
        CONTROL("itc" id, "Tail cut" label, U_MSEC, impulse_reverb_base_metadata::CONV_LENGTH), \
        CONTROL("ifi" id, "Fade in" label, U_MSEC, impulse_reverb_base_metadata::CONV_LENGTH), \
        CONTROL("ifo" id, "Fade out" label, U_MSEC, impulse_reverb_base_metadata::CONV_LENGTH), \
        TRIGGER("ils" id, "Impulse listen" label), \
        SWITCH("irv" id, "Impulse reverse" label, 0.0f), \
        STATUS("ifs" id, "Load status" label), \
        METER("ifl" id, "Impulse length" label, U_MSEC, impulse_reverb_base_metadata::CONV_LENGTH), \
        MESH("ifd" id, "Impulse file contents" label, impulse_reverb_base_metadata::TRACKS_MAX, impulse_reverb_base_metadata::MESH_SIZE)

    #define IR_CONVOLVER_MONO(id, label, mix) \
        COMBO("cs" id, "Channel source" label, 0, ir_source), \
        AMP_GAIN100("mk" id, "Makeup gain" label, 1.0f), \
        BLINK("ca" id, "Channel activity" label), \
        CONTROL("pd" id, "Channel pre-delay" label, U_MSEC, impulse_reverb_base_metadata::PREDELAY), \
        PAN_CTL("com" id, "Channel Left/Right output mix" label, mix)

    #define IR_CONVOLVER_STEREO(id, label, in_mix, out_mix) \
        PAN_CTL("cim" id, "Left/Right input mix" label, in_mix), \
        IR_CONVOLVER_MONO(id, label, out_mix)

    static const port_t impulse_reverb_mono_ports[] =
    {
        // Input audio ports
        AUDIO_INPUT_MONO,
        AUDIO_OUTPUT_LEFT,
        AUDIO_OUTPUT_RIGHT,
        IR_COMMON(IR_PAN_MONO),

        // Input controls
        IR_SAMPLE_FILE("0", " 0"),
        IR_SAMPLE_FILE("1", " 1"),
        IR_SAMPLE_FILE("2", " 2"),
        IR_SAMPLE_FILE("3", " 3"),
        IR_CONVOLVER_MONO("0", " 0", -100.0f),
        IR_CONVOLVER_MONO("1", " 1", +100.0f),
        IR_CONVOLVER_MONO("2", " 2", -100.0f),
        IR_CONVOLVER_MONO("3", " 3", +100.0f),

        PORTS_END
    };

    static const port_t impulse_reverb_stereo_ports[] =
    {
        // Input audio ports
        PORTS_STEREO_PLUGIN,
        IR_COMMON(IR_PAN_STEREO),

        // Input controls
        IR_SAMPLE_FILE("0", " 0"),
        IR_SAMPLE_FILE("1", " 1"),
        IR_SAMPLE_FILE("2", " 2"),
        IR_SAMPLE_FILE("3", " 3"),
        IR_CONVOLVER_STEREO("0", " 0", -100.0f, -100.0f),
        IR_CONVOLVER_STEREO("1", " 1", -100.0f, +100.0f),
        IR_CONVOLVER_STEREO("2", " 2", +100.0f, -100.0f),
        IR_CONVOLVER_STEREO("3", " 3", +100.0f, +100.0f),

        PORTS_END
    };

    static const int impulse_reverb_classes[] = { C_CONVERTER, C_SPECTRAL, -1 };

    const plugin_metadata_t  impulse_reverb_mono_metadata::metadata =
    {
        "Impulsnachhall Mono",
        "Impulse Reverb Mono",
        "INH1M",
        &developers::v_sadovnikov,
        "impulse_reverb_mono",
        "fggq",
        LSP_IMPULSE_REVERB_BASE + 0,
        LSP_VERSION(1, 0, 0),
        impulse_reverb_classes,
        impulse_reverb_mono_ports,
        "convolution/impulse_reverb/mono.xml",
        mono_to_stereo_plugin_port_groups
    };

    const plugin_metadata_t  impulse_reverb_stereo_metadata::metadata =
    {
        "Impulsnachhall Stereo",
        "Impulse Reverb Stereo",
        "INH1S",
        &developers::v_sadovnikov,
        "impulse_reverb_stereo",
        "o9zj",
        LSP_IMPULSE_REVERB_BASE + 1,
        LSP_VERSION(1, 0, 0),
        impulse_reverb_classes,
        impulse_reverb_stereo_ports,
        "convolution/impulse_reverb/stereo.xml",
        stereo_plugin_port_groups
    };
}



