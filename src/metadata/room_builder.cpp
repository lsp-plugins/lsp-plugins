/*
 * room_builder.cpp
 *
 *  Created on: 25 апр. 2019 г.
 *      Author: sadko
 */

#include <metadata/plugins.h>
#include <metadata/ports.h>
#include <metadata/developers.h>

namespace lsp
{
    static const char *rb_fft_rank[] =
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

    static const char *rb_view[] =
    {
        "Room browser",
        "Sample 0",
        "Sample 1",
        "Sample 2",
        "Sample 3",
        "Sample 4",
        "Sample 5",
        "Sample 6",
        "Sample 7",
        NULL
    };

    #define RB_PAN_MONO \
        PAN_CTL("p", "Panorama", 0.0f)

    #define RB_PAN_STEREO \
        PAN_CTL("pl", "Left channel panorama", -100.0f), \
        PAN_CTL("pr", "Right channel panorama", 100.0f)

    #define RB_COMMON(pan) \
        BYPASS, \
        COMBO("view", "Current view", 0, rb_view),      \
        COMBO("fft", "FFT size", room_builder_base_metadata::FFT_RANK_DEFAULT, rb_fft_rank), \
        CONTROL("pd", "Pre-delay", U_MSEC, room_builder_base_metadata::PREDELAY), \
        pan, \
        DRY_GAIN(1.0f), \
        WET_GAIN(1.0f), \
        OUT_GAIN, \
        PATH("ifn", "Input 3D model file name"),    \
        STATUS("ifs", "Input 3D model load status"), \
        METER_PERCENT("ifp", "File loading progress")

    static const port_t room_builder_mono_ports[] =
    {
        // Input audio ports
        AUDIO_INPUT_MONO,
        AUDIO_OUTPUT_LEFT,
        AUDIO_OUTPUT_RIGHT,
        RB_COMMON(RB_PAN_MONO),


        PORTS_END
    };

    static const port_t room_builder_stereo_ports[] =
    {
        // Input audio ports
        PORTS_STEREO_PLUGIN,
        RB_COMMON(RB_PAN_STEREO),

        PORTS_END
    };

    static const int room_builder_classes[] = { C_REVERB, -1 };

    const plugin_metadata_t  room_builder_mono_metadata::metadata =
    {
        "Raumbauer Mono",
        "Room Builder Mono",
        "RB1M",
        &developers::v_sadovnikov,
        "room_builder_mono",
        "????",
        0,
        LSP_VERSION(1, 0, 0),
        room_builder_classes,
        E_3D_BACKEND,
        room_builder_mono_ports,
        "convolution/room_builder/mono.xml",
        mono_to_stereo_plugin_port_groups
    };

    const plugin_metadata_t  room_builder_stereo_metadata::metadata =
    {
        "Raumbauer Stereo",
        "Room Builder Stereo",
        "RB1S",
        &developers::v_sadovnikov,
        "room_builder_stereo",
        "????",
        0,
        LSP_VERSION(1, 0, 0),
        room_builder_classes,
        E_3D_BACKEND,
        room_builder_stereo_ports,
        "convolution/room_builder/stereo.xml",
        stereo_plugin_port_groups
    };
}


