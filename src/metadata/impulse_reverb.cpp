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

    static const port_item_t ir_files[] =
    {
        { "None",       "file.none" },
        { "File 1",     "file.f1" },
        { "File 2",     "file.f2" },
        { "File 3",     "file.f3" },
        { "File 4",     "file.f4" },
        { NULL, NULL }
    };

    static const port_item_t ir_tracks[] =
    {
        { "Track 1",    "file.t1" },
        { "Track 2",    "file.t2" },
        { "Track 3",    "file.t3" },
        { "Track 4",    "file.t4" },
        { "Track 5",    "file.t5" },
        { "Track 6",    "file.t6" },
        { "Track 7",    "file.t7" },
        { "Track 8",    "file.t8" },
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
        { "File 3",     "file.f3" },
        { "File 4",     "file.f4" },
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

    #define IR_CONVOLVER_MONO(id, label, track, mix) \
        COMBO("csf" id, "Channel source file" label, 0, ir_files), \
        COMBO("cst" id, "Channel source track" label, track, ir_tracks), \
        AMP_GAIN100("mk" id, "Makeup gain" label, 1.0f), \
        SWITCH("cam" id, "Channel mute" label, 0.0f), \
        BLINK("ca" id, "Channel activity" label), \
        CONTROL("pd" id, "Channel pre-delay" label, U_MSEC, impulse_reverb_base_metadata::PREDELAY), \
        PAN_CTL("com" id, "Channel Left/Right output mix" label, mix)

    #define IR_CONVOLVER_STEREO(id, label, track, in_mix, out_mix) \
        PAN_CTL("cim" id, "Left/Right input mix" label, in_mix), \
        IR_CONVOLVER_MONO(id, label, track, out_mix)

    #define IR_EQ_BAND(id, freq)    \
        CONTROL("eq_" #id, "Band " freq "Hz gain", U_GAIN_AMP, impulse_reverb_base_metadata::BA)

    #define IR_EQUALIZER    \
        SWITCH("wpp", "Wet post-process", 0),    \
        COMBO("lcm", "Low-cut mode", 0, filter_slope),      \
        CONTROL("lcf", "Low-cut frequency", U_HZ, impulse_reverb_base_metadata::LCF),   \
        IR_EQ_BAND(0, "50"), \
        IR_EQ_BAND(1, "107"), \
        IR_EQ_BAND(2, "227"), \
        IR_EQ_BAND(3, "484"), \
        IR_EQ_BAND(4, "1 k"), \
        IR_EQ_BAND(5, "2.2 k"), \
        IR_EQ_BAND(6, "4.7 k"), \
        IR_EQ_BAND(7, "10 k"), \
        COMBO("hcm", "High-cut mode", 0, filter_slope),      \
        CONTROL("hcf", "High-cut frequency", U_HZ, impulse_reverb_base_metadata::HCF)

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
        IR_CONVOLVER_MONO("0", " 0", 0, -100.0f),
        IR_CONVOLVER_MONO("1", " 1", 1, +100.0f),
        IR_CONVOLVER_MONO("2", " 2", 0, -100.0f),
        IR_CONVOLVER_MONO("3", " 3", 1, +100.0f),

        // Impulse response equalizer
        IR_EQUALIZER,

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
        IR_CONVOLVER_STEREO("0", " 0", 0, -100.0f, -100.0f),
        IR_CONVOLVER_STEREO("1", " 1", 1, -100.0f, +100.0f),
        IR_CONVOLVER_STEREO("2", " 2", 2, +100.0f, -100.0f),
        IR_CONVOLVER_STEREO("3", " 3", 3, +100.0f, +100.0f),

        // Impulse response equalizer
        IR_EQUALIZER,

        PORTS_END
    };

    static const int impulse_reverb_classes[] = { C_REVERB, -1 };

    const plugin_metadata_t  impulse_reverb_mono_metadata::metadata =
    {
        "Impulsnachhall Mono",
        "Impulse Reverb Mono",
        "INH1M",
        &developers::v_sadovnikov,
        "impulse_reverb_mono",
        "fggq",
        0,
        LSP_VERSION(1, 0, 1),
        impulse_reverb_classes,
        E_NONE,
        impulse_reverb_mono_ports,
        "convolution/impulse_reverb/mono.xml",
        NULL,
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
        0,
        LSP_VERSION(1, 0, 1),
        impulse_reverb_classes,
        E_NONE,
        impulse_reverb_stereo_ports,
        "convolution/impulse_reverb/stereo.xml",
        NULL,
        stereo_plugin_port_groups
    };
}



