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

    static const char *rb_capture_config[] =
    {
        "Mono",
        "XY",
        "AB",
        "ORTF",
        "MS",
        NULL
    };

    static const char *rb_capture_direction[] =
    {
        "Cardioid",
        "Supercardioid",
        "Hypercardioid",
        "Bidirectional",
        "8-directional",
        "Omnidirectional",
        NULL
    };

    static const char *rb_capture_side_direction[] =
    {
        "Bidirectional",
        "8-directional",
        NULL
    };

    static const char *rb_reflection[] =
    {
        "Any",
        "0",
        "1",
        "2",
        "3",
        "4",
        "5",
        "6",
        "7",
        "8",
        "9",
        "10",
        "11",
        "12",
        "13",
        "14",
        "15",
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
        METER_PERCENT("ifp", "File loading progress"), \
        CONTROL("cposx", "Camera X position", U_M, room_builder_base_metadata::POSITION), \
        CONTROL("cposy", "Camera Y position", U_M, room_builder_base_metadata::POSITION), \
        CONTROL("cposz", "Camera Z position", U_M, room_builder_base_metadata::POSITION), \
        { "cyaw", "Camera Yaw angle", U_DEG, R_CONTROL, F_IN | F_LOWER | F_UPPER | F_STEP | F_CYCLIC, 0, 360, 0, 0.1f, NULL, NULL }, \
        { "cpitch", "Camera Pitch angle", U_DEG, R_CONTROL, F_IN | F_LOWER | F_UPPER | F_STEP, -89.0f, 89.0f, 0, 0.1f, NULL, NULL }

    #define RB_CAPTURE(id, label, ena) \
        SWITCH("sce" id, "Capture " label " enable", ena), \
        COMBO("scf" id, "Capture " label " first reflection order", 2, rb_reflection), \
        COMBO("scl" id, "Capture " label " last reflection order", 0, rb_reflection), \
        CONTROL("scpx" id, "Capture " label " X position", U_M, room_builder_base_metadata::POSITION), \
        CONTROL("scpy" id, "Capture " label " Y position", U_M, room_builder_base_metadata::POSITION), \
        CONTROL("scpz" id, "Capture " label " Z position", U_M, room_builder_base_metadata::POSITION), \
        { "scay" id , "Capture " label " Yaw angle", U_DEG, R_CONTROL, F_IN | F_LOWER | F_UPPER | F_STEP | F_CYCLIC, 0, 360, 0, 0.1f, NULL, NULL }, \
        { "scap" id , "Capture " label " Pitch angle", U_DEG, R_CONTROL, F_IN | F_LOWER | F_UPPER | F_STEP, -89.0f, 89.0f, 0, 0.1f, NULL, NULL }, \
        { "scar" id , "Capture " label " Roll angle", U_DEG, R_CONTROL, F_IN | F_LOWER | F_UPPER | F_STEP | F_CYCLIC, 0, 360, 0, 0.1f, NULL, NULL }, \
        CONTROL("sccs" id, "Capture " label " capsule size", U_MM, room_builder_base_metadata::CAPSULE), \
        COMBO("sccf" id, "Capture " label " configuration", 1, rb_capture_config),      \
        CONTROL("sca" id, "Capture " label " angle", U_DEG, room_builder_base_metadata::ANGLE),      \
        CONTROL("scab" id, "Capture " label " AB distance", U_M, room_builder_base_metadata::DISTANCE),      \
        COMBO("scmd" id, "Capture " label " microphone direction", 0, rb_capture_direction),      \
        COMBO("scsd" id, "Capture " label " side microphone direction", 0, rb_capture_side_direction)


    static const port_t room_builder_mono_ports[] =
    {
        // Input audio ports
        AUDIO_INPUT_MONO,
        AUDIO_OUTPUT_LEFT,
        AUDIO_OUTPUT_RIGHT,
        RB_COMMON(RB_PAN_MONO),

        RB_CAPTURE("0", "0", 1),
        RB_CAPTURE("1", "1", 0),
        RB_CAPTURE("2", "2", 0),
        RB_CAPTURE("3", "3", 0),
        RB_CAPTURE("4", "4", 0),
        RB_CAPTURE("5", "5", 0),
        RB_CAPTURE("6", "6", 0),
        RB_CAPTURE("7", "7", 0),

        PORTS_END
    };

    static const port_t room_builder_stereo_ports[] =
    {
        // Input audio ports
        PORTS_STEREO_PLUGIN,
        RB_COMMON(RB_PAN_STEREO),

        RB_CAPTURE("0", "0", 1),
        RB_CAPTURE("1", "1", 0),
        RB_CAPTURE("2", "2", 0),
        RB_CAPTURE("3", "3", 0),
        RB_CAPTURE("4", "4", 0),
        RB_CAPTURE("5", "5", 0),
        RB_CAPTURE("6", "6", 0),
        RB_CAPTURE("7", "7", 0),

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


