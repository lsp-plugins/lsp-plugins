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

    static const char *rb_editor[] =
    {
        "Source editor",
        "Capture editor",
        NULL
    };

    static const char *rb_ssel[] =
    {
        "0",
        "1",
        "2",
        "3",
        "4",
        "5",
        "6",
        "7",
        NULL
    };

    static const char *rb_csel[] =
    {
        "0",
        "1",
        "2",
        "3",
        "4",
        "5",
        "6",
        "7",
        NULL
    };

    static const char *rb_channel_sel[] =
    {
        "Left",
        "Right",
        NULL
    };

    static const char *rb_source_mode[] =
    {
        "Triangle",
        "Tetrahedron",
        "Octahedron",
        "Box",
        "Icosahedron",
        "Cylinder",
        "Cone",
        "Octasphere",
        "Icosphere",
        "Flat spot",
        "Cylindrical spot",
        "Spherical spot",
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

    static const char *rb_orientation[] =
    {
        "+X forward, +Y up",
        "+X forward, +Z up",
        "+X forward, -Y up",
        "+X forward, -Z up",
        "-X forward, +Y up",
        "-X forward, +Z up",
        "-X forward, -Y up",
        "-X forward, -Z up",

        "+Y forward, +X up",
        "+Y forward, +Z up",
        "+Y forward, -X up",
        "+Y forward, -Z up",
        "-Y forward, +X up",
        "-Y forward, +Z up",
        "-Y forward, -X up",
        "-Y forward, -Z up",

        "+Z forward, +X up",
        "+Z forward, +Y up",
        "+Z forward, -X up",
        "+Z forward, -Y up",
        "-Z forward, +X up",
        "-Z forward, +Y up",
        "-Z forward, -X up",
        "-Z forward, -Y up",

        NULL
    };

    #define RB_PAN_MONO \
        PAN_CTL("p", "Panorama", 0.0f)

    #define RB_PAN_STEREO \
        PAN_CTL("pl", "Left channel panorama", -100.0f), \
        PAN_CTL("pr", "Right channel panorama", 100.0f)

    #define RB_COMMON(pan) \
        BYPASS, \
        COMBO("view", "Current view", 0, rb_view),              \
        COMBO("editor", "Current editor", 0, rb_editor),        \
        COMBO("fft", "FFT size", room_builder_base_metadata::FFT_RANK_DEFAULT, rb_fft_rank), \
        CONTROL("pd", "Pre-delay", U_MSEC, room_builder_base_metadata::PREDELAY), \
        pan, \
        DRY_GAIN(1.0f), \
        WET_GAIN(1.0f), \
        OUT_GAIN, \
        PATH("ifn", "Input 3D model file name"),    \
        STATUS("ifs", "Input 3D model load status"), \
        METER_PERCENT("ifp", "File loading progress"), \
        COMBO("ifo", "Input 3D model orientation", 9, rb_orientation), \
        CONTROL_DFL("cposx", "Camera X position", U_M, room_builder_base_metadata::POSITION, 1.0f), \
        CONTROL_DFL("cposy", "Camera Y position", U_M, room_builder_base_metadata::POSITION, -0.2f), \
        CONTROL_DFL("cposz", "Camera Z position", U_M, room_builder_base_metadata::POSITION, 0.5f), \
        { "cyaw", "Camera Yaw angle", U_DEG, R_CONTROL, F_IN | F_LOWER | F_UPPER | F_STEP | F_CYCLIC, 0, 360, 80.0f, 0.1f, NULL, NULL }, \
        { "cpitch", "Camera Pitch angle", U_DEG, R_CONTROL, F_IN | F_LOWER | F_UPPER | F_STEP, -89.0f, 89.0f, -25.0f, 0.1f, NULL, NULL }

    #define RB_SOURCE_MONO(id, label, x, total, ena) \
        SWITCH("sse" id, "Source " label " enable", ena), \
        COMBO("sscf" id, "Source " label " type", 4, rb_source_mode), \
        SWITCH("ssph" id, "Source " label " phase invert", 0), \
        CONTROL_DFL("sspx" id, "Source " label " X position", U_M, room_builder_base_metadata::POSITION, 0.0f), \
        CONTROL_DFL("sspy" id, "Source " label " Y position", U_M, room_builder_base_metadata::POSITION, -1.0f), \
        CONTROL_DFL("sspz" id, "Source " label " Z position", U_M, room_builder_base_metadata::POSITION, 0.0f), \
        { "ssay" id , "Source " label " Yaw angle", U_DEG, R_CONTROL, F_IN | F_LOWER | F_UPPER | F_STEP | F_CYCLIC, 0.0f, 360, 90.0f, 0.1f, NULL, NULL }, \
        { "ssap" id , "Source " label " Pitch angle", U_DEG, R_CONTROL, F_IN | F_LOWER | F_UPPER | F_STEP, -90.0f, 90.0f, 0, 0.1f, NULL, NULL }, \
        { "ssar" id , "Source " label " Roll angle", U_DEG, R_CONTROL, F_IN | F_LOWER | F_UPPER | F_STEP | F_CYCLIC, 0, 360, 0, 0.1f, NULL, NULL }, \
        CONTROL("sss" id, "Source " label " size", U_CM, room_builder_base_metadata::SOURCE), \
        CONTROL("shh" id, "Source " label " height", U_CM, room_builder_base_metadata::HEIGHT), \
        PERCENTS("ssa" id, "Source " label " angle", 50.0f, 0.025f), \
        PERCENTS("sscv" id, "Source " label " curvature", 100.0f, 0.05f), \
        { "ssh" id, "Source " label " hue", U_NONE, R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP | F_CYCLIC, 0.0f, 1.0f, (float(x) / float(total)), 0.25f/360.0f, NULL     }

    #define RB_SOURCE_STEREO(id, label, x, total, ena, channel) \
        COMBO("ssch" id, "Source " label " channel", channel, rb_channel_sel), \
        RB_SOURCE_MONO(id, label, x, total, ena)

    #define RB_CAPTURE(id, label, x, total, ena) \
        SWITCH("sce" id, "Capture " label " enable", ena), \
        COMBO("scf" id, "Capture " label " first reflection order", 2, rb_reflection), \
        COMBO("scl" id, "Capture " label " last reflection order", 0, rb_reflection), \
        CONTROL_DFL("scpx" id, "Capture " label " X position", U_M, room_builder_base_metadata::POSITION, 0.0f), \
        CONTROL_DFL("scpy" id, "Capture " label " Y position", U_M, room_builder_base_metadata::POSITION, 1.0f), \
        CONTROL_DFL("scpz" id, "Capture " label " Z position", U_M, room_builder_base_metadata::POSITION, 0.0f), \
        { "scay" id , "Capture " label " Yaw angle", U_DEG, R_CONTROL, F_IN | F_LOWER | F_UPPER | F_STEP | F_CYCLIC, 0, 360, 270, 0.1f, NULL, NULL }, \
        { "scap" id , "Capture " label " Pitch angle", U_DEG, R_CONTROL, F_IN | F_LOWER | F_UPPER | F_STEP, -90.0f, 90.0f, 0, 0.1f, NULL, NULL }, \
        { "scar" id , "Capture " label " Roll angle", U_DEG, R_CONTROL, F_IN | F_LOWER | F_UPPER | F_STEP | F_CYCLIC, 0, 360, 0, 0.1f, NULL, NULL }, \
        CONTROL("sccs" id, "Capture " label " capsule size", U_CM, room_builder_base_metadata::CAPSULE), \
        COMBO("sccf" id, "Capture " label " configuration", 1, rb_capture_config),      \
        CONTROL("sca" id, "Capture " label " angle", U_DEG, room_builder_base_metadata::ANGLE),      \
        CONTROL("scab" id, "Capture " label " AB distance", U_M, room_builder_base_metadata::DISTANCE),      \
        COMBO("scmd" id, "Capture " label " microphone direction", 0, rb_capture_direction),      \
        COMBO("scsd" id, "Capture " label " side microphone direction", 0, rb_capture_side_direction), \
        { "sch" id, "Capture " label " hue", U_NONE, R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP | F_CYCLIC, 0.0f, 1.0f, (float(x) / float(total)), 0.25f/360.0f, NULL     }

    static const port_t room_builder_mono_ports[] =
    {
        // Input audio ports
        AUDIO_INPUT_MONO,
        AUDIO_OUTPUT_LEFT,
        AUDIO_OUTPUT_RIGHT,
        PORTS_OSC_CHANNEL,
        RB_COMMON(RB_PAN_MONO),

        COMBO("ssel", "Source selector", 0, rb_ssel),
        RB_SOURCE_MONO("_0", "0", 0, 8, 1),
        RB_SOURCE_MONO("_1", "1", 1, 8, 0),
        RB_SOURCE_MONO("_2", "2", 2, 8, 0),
        RB_SOURCE_MONO("_3", "3", 3, 8, 0),
        RB_SOURCE_MONO("_4", "4", 4, 8, 0),
        RB_SOURCE_MONO("_5", "5", 5, 8, 0),
        RB_SOURCE_MONO("_6", "6", 6, 8, 0),
        RB_SOURCE_MONO("_7", "7", 7, 8, 0),

        COMBO("csel", "Capture selector", 0, rb_csel),
        RB_CAPTURE("_0", "0", 0, 8, 1),
        RB_CAPTURE("_1", "1", 1, 8, 0),
        RB_CAPTURE("_2", "2", 2, 8, 0),
        RB_CAPTURE("_3", "3", 3, 8, 0),
        RB_CAPTURE("_4", "4", 4, 8, 0),
        RB_CAPTURE("_5", "5", 5, 8, 0),
        RB_CAPTURE("_6", "6", 6, 8, 0),
        RB_CAPTURE("_7", "7", 7, 8, 0),

        PORTS_END
    };

    static const port_t room_builder_stereo_ports[] =
    {
        // Input audio ports
        PORTS_STEREO_PLUGIN,
        PORTS_OSC_CHANNEL,
        RB_COMMON(RB_PAN_STEREO),

        COMBO("ssel", "Source selector", 0, rb_ssel),
        RB_SOURCE_STEREO("_0", "0", 0, 8, 1, 0),
        RB_SOURCE_STEREO("_1", "1", 1, 8, 1, 1),
        RB_SOURCE_STEREO("_2", "2", 2, 8, 0, 0),
        RB_SOURCE_STEREO("_3", "3", 3, 8, 0, 1),
        RB_SOURCE_STEREO("_4", "4", 4, 8, 0, 0),
        RB_SOURCE_STEREO("_5", "5", 5, 8, 0, 1),
        RB_SOURCE_STEREO("_6", "6", 6, 8, 0, 0),
        RB_SOURCE_STEREO("_7", "7", 7, 8, 0, 1),

        COMBO("csel", "Capture selector", 0, rb_csel),
        RB_CAPTURE("_0", "0", 0, 8, 1),
        RB_CAPTURE("_1", "1", 1, 8, 0),
        RB_CAPTURE("_2", "2", 2, 8, 0),
        RB_CAPTURE("_3", "3", 3, 8, 0),
        RB_CAPTURE("_4", "4", 4, 8, 0),
        RB_CAPTURE("_5", "5", 5, 8, 0),
        RB_CAPTURE("_6", "6", 6, 8, 0),
        RB_CAPTURE("_7", "7", 7, 8, 0),

        PORTS_END
    };

    static const int room_builder_classes[] = { C_REVERB, -1 };

    const port_t room_builder_base_metadata::osc_ports[] =
    {
        CONTROL_DFL("osc:xpos", "Object position X", U_M, room_builder_base_metadata::POSITION, 0.0f),
        CONTROL_DFL("osc:ypos", "Object position Y", U_M, room_builder_base_metadata::POSITION, 0.0f),
        CONTROL_DFL("osc:zpos", "Object position Z", U_M, room_builder_base_metadata::POSITION, 0.0f),
        { "osc:yaw", "Object Yaw angle", U_DEG, R_CONTROL, F_IN | F_LOWER | F_UPPER | F_STEP | F_CYCLIC, 0.0f, 360, 90.0f, 0.1f, NULL, NULL },
        { "osc:pitch", "Object Pitch angle", U_DEG, R_CONTROL, F_IN | F_LOWER | F_UPPER | F_STEP, -90.0f, 90.0f, 0, 0.1f, NULL, NULL },
        { "osc:roll", "Object Roll angle", U_DEG, R_CONTROL, F_IN | F_LOWER | F_UPPER | F_STEP | F_CYCLIC, 0, 360, 0, 0.1f, NULL, NULL },
        CONTROL("osc:xsize", "Object size X", U_M, room_builder_base_metadata::OSIZE),
        CONTROL("osc:ysize", "Object size Y", U_M, room_builder_base_metadata::OSIZE),
        CONTROL("osc:zsize", "Object size Z", U_M, room_builder_base_metadata::OSIZE),
        { "osc:hue", "Object hue", U_NONE, R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP | F_CYCLIC, 0.0f, 1.0f, 0.0f, 0.25f/360.0f, NULL     },
        CONTROL("osc:oabs", "Outer absorption", U_M, room_builder_base_metadata::MAT_ABSORPTION),
        CONTROL("osc:iabs", "Inner absorption", U_M, room_builder_base_metadata::MAT_ABSORPTION),
        CONTROL("osc:odisp", "Reflected wave outer dispersion", U_M, room_builder_base_metadata::MAT_DISPERSION),
        CONTROL("osc:idisp", "Reflected wave inner dispersion", U_M, room_builder_base_metadata::MAT_DISPERSION),
        CONTROL("osc:odiss", "Refracted wave outer dissipation", U_M, room_builder_base_metadata::MAT_DISPERSION),
        CONTROL("osc:idiss", "Refracted wave inner dissipation", U_M, room_builder_base_metadata::MAT_DISPERSION),
        CONTROL("osc:otransp", "Material outer transparency", U_M, room_builder_base_metadata::MAT_TRANSPARENCY),
        CONTROL("osc:itransp", "Material inner transparency", U_M, room_builder_base_metadata::MAT_TRANSPARENCY),
        CONTROL("osc:speed", "Sound speed in material", U_MPS, room_builder_base_metadata::MAT_SOUND_SPEED),
        COMBO("osc:oid", "Selected object index", 0, NULL)
    };

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
        E_3D_BACKEND | E_OSC,
        room_builder_mono_ports,
        "simulation/room_builder/mono.xml",
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
        E_3D_BACKEND | E_OSC,
        room_builder_stereo_ports,
        "simulation/room_builder/stereo.xml",
        stereo_plugin_port_groups
    };
}


