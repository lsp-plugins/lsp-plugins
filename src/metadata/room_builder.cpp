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
    static const port_item_t rb_fft_rank[] =
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

    static const port_item_t rb_view[] =
    {
        { "Room browser",   "room_bld.room_browser" },
        { "Sample 0",       "sampler.samp.0" },
        { "Sample 1",       "sampler.samp.1" },
        { "Sample 2",       "sampler.samp.2" },
        { "Sample 3",       "sampler.samp.3" },
        { "Sample 4",       "sampler.samp.4" },
        { "Sample 5",       "sampler.samp.5" },
        { "Sample 6",       "sampler.samp.6" },
        { "Sample 7",       "sampler.samp.7" },
        { NULL, NULL }
    };

    static const port_item_t rb_samples[] =
    {
        { "None",           "sampler.samp.none" },
        { "Sample 0",       "sampler.samp.0" },
        { "Sample 1",       "sampler.samp.1" },
        { "Sample 2",       "sampler.samp.2" },
        { "Sample 3",       "sampler.samp.3" },
        { "Sample 4",       "sampler.samp.4" },
        { "Sample 5",       "sampler.samp.5" },
        { "Sample 6",       "sampler.samp.6" },
        { "Sample 7",       "sampler.samp.7" },
        { NULL, NULL }
    };

    static const port_item_t rb_tracks[] =
    {
        { "Left",           "file.left" },
        { "Right",          "file.right" },
        { NULL, NULL }
    };

    static const port_item_t filter_slope[] =
    {
        { "off",            "eq.slope.off" },
        { "6 dB/oct",       "eq.slope.6dbo" },
        { "12 dB/oct",      "eq.slope.12dbo" },
        { "18 dB/oct",      "eq.slope.18dbo" },
        { NULL, NULL }
    };

    static const port_item_t rb_editor[] =
    {
        { "Source editor",      "room_bld.ed.source" },
        { "Capture editor",     "room_bld.ed.capture" },
        { "Object editor",      "room_bld.ed.object" },
        { "Material editor",    "room_bld.ed.material" },
        { NULL, NULL }
    };

    static const port_item_t rb_processors[] =
    {
        { "Convolvers",         "room_bld.convolvers" },
        { "IR Equalizer",       "room_bld.ir_eq" },
        { NULL, NULL }
    };

    static const port_item_t rb_ssel[] =
    {
        { "0", NULL },
        { "1", NULL },
        { "2", NULL },
        { "3", NULL },
        { "4", NULL },
        { "5", NULL },
        { "6", NULL },
        { "7", NULL },
        { NULL, NULL }
    };

    static const port_item_t rb_csel[] =
    {
        { "0", NULL },
        { "1", NULL },
        { "2", NULL },
        { "3", NULL },
        { "4", NULL },
        { "5", NULL },
        { "6", NULL },
        { "7", NULL },
        { NULL, NULL }
    };

    static const port_item_t rb_source_mode[] =
    {
        { "Triangle",           "room_bld.src.tri" },
        { "Tetrahedron",        "room_bld.src.tetra" },
        { "Octahedron",         "room_bld.src.octa" },
        { "Box",                "room_bld.src.box" },
        { "Icosahedron",        "room_bld.src.ico" },
        { "Cylinder",           "room_bld.src.cyl" },
        { "Cone",               "room_bld.src.cone" },
        { "Octasphere",         "room_bld.src.octa_s" },
        { "Icosphere",          "room_bld.src.ico_s" },
        { "Flat spot",          "room_bld.src.flat_s" },
        { "Cylindrical spot",   "room_bld.src.cyl_s" },
        { "Spherical spot",     "room_bld.src.sphe_s" },
        { NULL, NULL }
    };

    static const port_item_t rb_capture_config[] =
    {
        { "Mono",               "room_bld.capt.mono" },
        { "XY",                 "room_bld.capt.xy" },
        { "AB",                 "room_bld.capt.ab" },
        { "ORTF",               "room_bld.capt.ortf" },
        { "MS",                 "room_bld.capt.ms" },
        { NULL, NULL }
    };

    static const port_item_t rb_capture_direction[] =
    {
        { "Cardioid",           "room_bld.dir.cardio" },
        { "Supercardioid",      "room_bld.dir.superc" },
        { "Hypercardioid",      "room_bld.dir.hyperc" },
        { "Bidirectional",      "room_bld.dir.bi" },
        { "8-directional",      "room_bld.dir.eight" },
        { "Omnidirectional",    "room_bld.dir.omni" },
        { NULL, NULL }
    };

    static const port_item_t rb_capture_side_direction[] =
    {
        { "Bidirectional",      "room_bld.dir.bi" },
        { "8-directional",      "room_bld.dir.eight" },
        { NULL, NULL }
    };

    static const port_item_t rb_reflection[] =
    {
        { "Any",                "room_bld.refl_any" },
        { "0", NULL },
        { "1", NULL },
        { "2", NULL },
        { "3", NULL },
        { "4", NULL },
        { "5", NULL },
        { "6", NULL },
        { "7", NULL },
        { "8", NULL },
        { "9", NULL },
        { "10", NULL },
        { "11", NULL },
        { "12", NULL },
        { "13", NULL },
        { "14", NULL },
        { "15", NULL },
        { NULL, NULL }
    };

    static const port_item_t rb_orientation[] =
    {
        { "+X forward, +Y up",  "room_bld.orient.+x+y" },
        { "+X forward, +Z up",  "room_bld.orient.+x+z" },
        { "+X forward, -Y up",  "room_bld.orient.+x-y" },
        { "+X forward, -Z up",  "room_bld.orient.+x-z" },
        { "-X forward, +Y up",  "room_bld.orient.-x+y" },
        { "-X forward, +Z up",  "room_bld.orient.-x+z" },
        { "-X forward, -Y up",  "room_bld.orient.-x-y" },
        { "-X forward, -Z up",  "room_bld.orient.-x-z" },

        { "+Y forward, +X up",  "room_bld.orient.+y+x" },
        { "+Y forward, +Z up",  "room_bld.orient.+y+z" },
        { "+Y forward, -X up",  "room_bld.orient.+y-x" },
        { "+Y forward, -Z up",  "room_bld.orient.+y-z" },
        { "-Y forward, +X up",  "room_bld.orient.-y+x" },
        { "-Y forward, +Z up",  "room_bld.orient.-y+z" },
        { "-Y forward, -X up",  "room_bld.orient.-y-x" },
        { "-Y forward, -Z up",  "room_bld.orient.-y-z" },

        { "+Z forward, +X up",  "room_bld.orient.+z+x" },
        { "+Z forward, +Y up",  "room_bld.orient.+z+y" },
        { "+Z forward, -X up",  "room_bld.orient.+z-x" },
        { "+Z forward, -Y up",  "room_bld.orient.+z-y" },
        { "-Z forward, +X up",  "room_bld.orient.-z+x" },
        { "-Z forward, +Y up",  "room_bld.orient.-z+y" },
        { "-Z forward, -X up",  "room_bld.orient.-z-x" },
        { "-Z forward, -Y up",  "room_bld.orient.-z-y" },

        { NULL, NULL }
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
        COMBO("signal", "Current signal processor", 0, rb_processors),  \
        COMBO("fft", "FFT size", room_builder_base_metadata::FFT_RANK_DEFAULT, rb_fft_rank), \
        CONTROL("pd", "Pre-delay", U_MSEC, room_builder_base_metadata::PREDELAY), \
        pan, \
        AMP_GAIN1000("dry", "Dry amount", 1.0f), \
        AMP_GAIN1000("wet", "Wet amount", 1.0f), \
        OUT_GAIN, \
        CONTROL("threads", "Number of threads for processing", U_NONE, room_builder_base_metadata::THREADS), \
        PERCENTS("quality", "Quality factor", 50.0f, 0.1f), \
        STATUS("status", "Render status"), \
        OUT_PERCENTS("prog", "Rendering progress"), \
        SWITCH("normal", "Normalize rendered samples", 1.0f), \
        TRIGGER("render", "Launch/Stop rendering process"), \
        PATH("ifn", "Input 3D model file name"),    \
        STATUS("ifs", "Input 3D model load status"), \
        METER_PERCENT("ifp", "File loading progress"), \
        COMBO("ifo", "Input 3D model orientation", 9, rb_orientation), \
        CONTROL("xscale", "Scene X scale", U_PERCENT, room_builder_base_metadata::OSIZE), \
        CONTROL("yscale", "Scene Y scale", U_PERCENT, room_builder_base_metadata::OSIZE), \
        CONTROL("zscale", "Scene Z scale", U_PERCENT, room_builder_base_metadata::OSIZE), \
        CONTROL_DFL("cposx", "Camera X position", U_M, room_builder_base_metadata::POSITION, 1.0f), \
        CONTROL_DFL("cposy", "Camera Y position", U_M, room_builder_base_metadata::POSITION, -0.2f), \
        CONTROL_DFL("cposz", "Camera Z position", U_M, room_builder_base_metadata::POSITION, 0.5f), \
        { "cyaw", "Camera Yaw angle", U_DEG, R_CONTROL, F_IN | F_LOWER | F_UPPER | F_STEP | F_CYCLIC, 0, 360, 80.0f, 0.1f, NULL, NULL }, \
        { "cpitch", "Camera Pitch angle", U_DEG, R_CONTROL, F_IN | F_LOWER | F_UPPER | F_STEP, -89.0f, 89.0f, -25.0f, 0.1f, NULL, NULL }


    #define RB_SOURCE(id, label, x, total, ena) \
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
        \
        CONTROL("ihc" id, "Head cut" label, U_MSEC, room_builder_base_metadata::CONV_LENGTH), \
        CONTROL("itc" id, "Tail cut" label, U_MSEC, room_builder_base_metadata::CONV_LENGTH), \
        CONTROL("ifi" id, "Fade in" label, U_MSEC, room_builder_base_metadata::CONV_LENGTH), \
        CONTROL("ifo" id, "Fade out" label, U_MSEC, room_builder_base_metadata::CONV_LENGTH), \
        TRIGGER("ils" id, "Impulse listen" label), \
        SWITCH("irv" id, "Impulse reverse" label, 0.0f), \
        AMP_GAIN_RANGE("imkp" id, "Impulse makeup gain" label, 1.0f, 0.001f, 1000.0f), \
        STATUS("ifs" id, "Impulse status" label), \
        METER("ifl" id, "Impulse length" label, U_MSEC, room_builder_base_metadata::CONV_LENGTH), \
        METER("sdur" id, "Impulse current duration" label, U_MSEC, room_builder_base_metadata::DURATION), \
        METER("smdur" id, "Impulse max duration" label, U_MSEC, room_builder_base_metadata::DURATION), \
        MESH("ifd" id, "Impulse file contents" label, room_builder_base_metadata::TRACKS_MAX, room_builder_base_metadata::MESH_SIZE), \
        PATH("ofn" id, "Sample file name" label), \
        TRIGGER("ofc" id , "Sample save command" label), \
        STATUS("ofs" id, "Sample saving status" label), \
        METER_PERCENT("ofp" id, "Sample saving progress" label), \
        \
        { "sch" id, "Capture " label " hue", U_NONE, R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP | F_CYCLIC, 0.0f, 1.0f, (float(x) / float(total)), 0.25f/360.0f, NULL     }

    #define RB_CONVOLVER_MONO(id, label, track, mix) \
        COMBO("csf" id, "Channel source sample" label, 0, rb_samples), \
        COMBO("cst" id, "Channel source track" label, track, rb_tracks), \
        AMP_GAIN100("mk" id, "Makeup gain" label, 1.0f), \
        SWITCH("cam" id, "Channel mute" label, 0.0f), \
        BLINK("ca" id, "Channel activity" label), \
        CONTROL("pd" id, "Channel pre-delay" label, U_MSEC, impulse_reverb_base_metadata::PREDELAY), \
        PAN_CTL("com" id, "Channel Left/Right output mix" label, mix)

    #define RB_CONVOLVER_STEREO(id, label, track, in_mix, out_mix) \
        PAN_CTL("cim" id, "Left/Right input mix" label, in_mix), \
        RB_CONVOLVER_MONO(id, label, track, out_mix)

    #define RB_EQ_BAND(id, freq)    \
        CONTROL("eq_" #id, "Band " freq "Hz gain", U_GAIN_AMP, room_builder_base_metadata::BA)

    #define RB_EQUALIZER    \
        SWITCH("wpp", "Wet post-process", 0),    \
        COMBO("lcm", "Low-cut mode", 0, filter_slope),      \
        CONTROL("lcf", "Low-cut frequency", U_HZ, room_builder_base_metadata::LCF),   \
        RB_EQ_BAND(0, "50"), \
        RB_EQ_BAND(1, "107"), \
        RB_EQ_BAND(2, "227"), \
        RB_EQ_BAND(3, "484"), \
        RB_EQ_BAND(4, "1 k"), \
        RB_EQ_BAND(5, "2.2 k"), \
        RB_EQ_BAND(6, "4.7 k"), \
        RB_EQ_BAND(7, "10 k"), \
        COMBO("hcm", "High-cut mode", 0, filter_slope),      \
        CONTROL("hcf", "High-cut frequency", U_HZ, room_builder_base_metadata::HCF)

    static const port_t room_builder_mono_ports[] =
    {
        // Input audio ports
        AUDIO_INPUT_MONO,
        AUDIO_OUTPUT_LEFT,
        AUDIO_OUTPUT_RIGHT,
        RB_COMMON(RB_PAN_MONO),

        COMBO("ssel", "Source selector", 0, rb_ssel),
        RB_SOURCE("_0", "0", 0, 8, 1),
        RB_SOURCE("_1", "1", 1, 8, 0),
        RB_SOURCE("_2", "2", 2, 8, 0),
        RB_SOURCE("_3", "3", 3, 8, 0),
        RB_SOURCE("_4", "4", 4, 8, 0),
        RB_SOURCE("_5", "5", 5, 8, 0),
        RB_SOURCE("_6", "6", 6, 8, 0),
        RB_SOURCE("_7", "7", 7, 8, 0),

        COMBO("csel", "Capture selector", 0, rb_csel),
        RB_CAPTURE("_0", "0", 0, 8, 1),
        RB_CAPTURE("_1", "1", 1, 8, 0),
        RB_CAPTURE("_2", "2", 2, 8, 0),
        RB_CAPTURE("_3", "3", 3, 8, 0),
        RB_CAPTURE("_4", "4", 4, 8, 0),
        RB_CAPTURE("_5", "5", 5, 8, 0),
        RB_CAPTURE("_6", "6", 6, 8, 0),
        RB_CAPTURE("_7", "7", 7, 8, 0),

        RB_CONVOLVER_MONO("0", " 0", 0, -100.0f),
        RB_CONVOLVER_MONO("1", " 1", 1, +100.0f),
        RB_CONVOLVER_MONO("2", " 2", 0, -100.0f),
        RB_CONVOLVER_MONO("3", " 3", 1, +100.0f),

        RB_EQUALIZER,

        PORTS_END
    };

    static const port_t room_builder_stereo_ports[] =
    {
        // Input audio ports
        PORTS_STEREO_PLUGIN,
        RB_COMMON(RB_PAN_STEREO),

        COMBO("ssel", "Source selector", 0, rb_ssel),
        RB_SOURCE("_0", "0", 0, 8, 1),
        RB_SOURCE("_1", "1", 1, 8, 0),
        RB_SOURCE("_2", "2", 2, 8, 0),
        RB_SOURCE("_3", "3", 3, 8, 0),
        RB_SOURCE("_4", "4", 4, 8, 0),
        RB_SOURCE("_5", "5", 5, 8, 0),
        RB_SOURCE("_6", "6", 6, 8, 0),
        RB_SOURCE("_7", "7", 7, 8, 0),

        COMBO("csel", "Capture selector", 0, rb_csel),
        RB_CAPTURE("_0", "0", 0, 8, 1),
        RB_CAPTURE("_1", "1", 1, 8, 0),
        RB_CAPTURE("_2", "2", 2, 8, 0),
        RB_CAPTURE("_3", "3", 3, 8, 0),
        RB_CAPTURE("_4", "4", 4, 8, 0),
        RB_CAPTURE("_5", "5", 5, 8, 0),
        RB_CAPTURE("_6", "6", 6, 8, 0),
        RB_CAPTURE("_7", "7", 7, 8, 0),

        RB_CONVOLVER_STEREO("0", " 0", 0, -100.0f, -100.0f),
        RB_CONVOLVER_STEREO("1", " 1", 1, -100.0f, +100.0f),
        RB_CONVOLVER_STEREO("2", " 2", 0, +100.0f, -100.0f),
        RB_CONVOLVER_STEREO("3", " 3", 1, +100.0f, +100.0f),

        RB_EQUALIZER,

        PORTS_END
    };

    static const int room_builder_classes[] = { C_REVERB, -1 };

    const port_t room_builder_base_metadata::kvt_ports[] =
    {
        COMBO("kvt:oid", "Selected object index", 0, NULL),
        SWITCH("kvt:enabled", "Object enable", 0),
        CONTROL_DFL("kvt:xpos", "Object position X", U_M, room_builder_base_metadata::POSITION, 0.0f),
        CONTROL_DFL("kvt:ypos", "Object position Y", U_M, room_builder_base_metadata::POSITION, 0.0f),
        CONTROL_DFL("kvt:zpos", "Object position Z", U_M, room_builder_base_metadata::POSITION, 0.0f),
        { "kvt:yaw", "Object Yaw angle", U_DEG, R_CONTROL, F_IN | F_LOWER | F_UPPER | F_STEP | F_CYCLIC, 0.0f, 360, 0.0f, 0.1f, NULL, NULL },
        { "kvt:pitch", "Object Pitch angle", U_DEG, R_CONTROL, F_IN | F_LOWER | F_UPPER | F_STEP, -90.0f, 90.0f, 0, 0.1f, NULL, NULL },
        { "kvt:roll", "Object Roll angle", U_DEG, R_CONTROL, F_IN | F_LOWER | F_UPPER | F_STEP | F_CYCLIC, 0, 360, 0, 0.1f, NULL, NULL },
        CONTROL("kvt:xscale", "Object scaling X", U_PERCENT, room_builder_base_metadata::OSIZE),
        CONTROL("kvt:yscale", "Object scaling Y", U_PERCENT, room_builder_base_metadata::OSIZE),
        CONTROL("kvt:zscale", "Object scaling Z", U_PERCENT, room_builder_base_metadata::OSIZE),
        { "kvt:hue", "Object hue", U_NONE, R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP | F_CYCLIC, 0.0f, 1.0f, 0.0f, 0.25f/360.0f, NULL     },
        LOG_CONTROL("kvt:oabs", "Outer absorption", U_PERCENT, room_builder_base_metadata::MAT_ABSORPTION),
        LOG_CONTROL("kvt:iabs", "Inner absorption", U_PERCENT, room_builder_base_metadata::MAT_ABSORPTION),
        SWITCH("kvt:labs", "Link absorption parameters", 1.0f),
        LOG_CONTROL("kvt:odisp", "Refracted wave outer dispersion", U_NONE, room_builder_base_metadata::MAT_DISPERSION),
        LOG_CONTROL("kvt:idisp", "Refracted wave inner dispersion", U_NONE, room_builder_base_metadata::MAT_DISPERSION),
        SWITCH("kvt:ldisp", "Link refracted wave dispersion parameters", 1.0f),
        LOG_CONTROL("kvt:odiff", "Reflected wave outer diffusion", U_NONE, room_builder_base_metadata::MAT_DISPERSION),
        LOG_CONTROL("kvt:idiff", "Reflected wave inner diffusion", U_NONE, room_builder_base_metadata::MAT_DISPERSION),
        SWITCH("kvt:ldiff", "Link reflected wave inner diffusion parameters", 1.0f),
        CONTROL("kvt:otransp", "Material outer transparency", U_NONE, room_builder_base_metadata::MAT_TRANSPARENCY),
        CONTROL("kvt:itransp", "Material inner transparency", U_NONE, room_builder_base_metadata::MAT_TRANSPARENCY),
        SWITCH("kvt:ltransp", "Link material transparency parameters", 1.0f),
        CONTROL("kvt:speed", "Sound speed in material", U_MPS, room_builder_base_metadata::MAT_SOUND_SPEED)
    };

    const room_material_t room_builder_base_metadata::materials[] =
    {
        { "Alder",          "room_bld.mat.alder",           5060.0f,    6.0f    },
        { "Aluminum",       "room_bld.mat.aluminium",       5080.0f,    0.0f    },
        { "Ash",            "room_bld.mat.ash",             5065.0f,    6.0f    },
        { "Basalt Fiber",   "room_bld.mat.basalt_fiber",    340.29f,    95.0f   },
        { "Birch",          "room_bld.mat.birch",           3625.0f,    8.0f    },
        { "Brass",          "room_bld.mat.brass",           3490.0f,    0.0f    },
        { "Brick",          "room_bld.mat.brick",           3600.0f,    3.2f    },
        { "Cast Iron",      "room_bld.mat.cast_iron",       3850.0f,    0.1f    },
        { "Concrete",       "room_bld.mat.concrete",        4250.0f,    1.5f    },
        { "Copper",         "room_bld.mat.copper",          3710.0f,    0.0f    },
        { "Corkwood",       "room_bld.mat.corkwood",        500.0f,     10.0f   },
        { "Cotton",         "room_bld.mat.cotton",          340.29f,    17.0f   },
        { "Fiberboard",     "room_bld.mat.fiberboard",      340.29f,    70.0f   },
        { "Fiberglass",     "room_bld.mat.fiberglass",      340.29f,    75.0f   },
        { "Fir",            "room_bld.mat.fir",             4600.0f,    6.0f    },
        { "Fresh Water",    "room_bld.mat.fresh_water",     1493.0f,    0.0f    },
        { "Glass",          "room_bld.mat.glass",           5370.0f,    3.0f    },
        { "Granite",        "room_bld.mat.granite",         3950.0f,    1.5f    },
        { "Gypsum",         "room_bld.mat.gypsum",          4970.0f,    6.2f    },
        { "Ice",            "room_bld.mat.ice",             3280.0f,    0.0f    },
        { "Iron",           "room_bld.mat.iron",            5170.0f,    0.0f    },
        { "Lead",           "room_bld.mat.lead",            1200.0f,    0.2f    },
        { "Maple",          "room_bld.mat.maple",           4450.0f,    6.0f    },
        { "Marble",         "room_bld.mat.marble",          6150.0f,    1.3f    },
        { "Mineral Wool",   "room_bld.mat.mineral_wool",    340.29f,    87.0f   },
        { "Nickel",         "room_bld.mat.nickel",          4785.0f,    0.0f    },
        { "Nickel Silver",  "room_bld.mat.nickel_silver",   3580.0f,    0.0f    },
        { "Oak",            "room_bld.mat.oak",             4050.0f,    7.0f    },
        { "Pine",           "room_bld.mat.pine",            5030.0f,    6.0f    },
        { "Plexiglass",     "room_bld.mat.plexiglass",      2670.0f,    15.0f   },
        { "Polystyrene",    "room_bld.mat.polystyrene",     2350.0f,    4.0f    },
        { "Rubber",         "room_bld.mat.rubber",          1600.0f,    6.0f    },
        { "Sea Water",      "room_bld.mat.sea_water",       1533.0f,    0.0f    },
        { "Silicon",        "room_bld.mat.silocon",         3770.0f,    2.0f    },
        { "Slate",          "room_bld.mat.slate",           4510.0f,    1.9f    },
        { "Steel",          "room_bld.mat.steel",           5050.0f,    0.0f    },
        { "Tin",            "room_bld.mat.tin",             2730.0f,    0.1f    },
        { "Velvet",         "room_bld.mat.velvet",          340.29f,    45.0f   },
        { NULL,             NULL,                           0.0f,       0.0f    }
    };

    const plugin_metadata_t  room_builder_mono_metadata::metadata =
    {
        "Raumbaumeister Mono",
        "Room Builder Mono",
        "RB1M",
        &developers::v_sadovnikov,
        "room_builder_mono",
        "cqbr",
        0,
        LSP_VERSION(1, 0, 1),
        room_builder_classes,
        E_3D_BACKEND | E_KVT_SYNC,
        room_builder_mono_ports,
        "simulation/room_builder/mono.xml",
        "simulation/room_builder",
        mono_to_stereo_plugin_port_groups
    };

    const plugin_metadata_t  room_builder_stereo_metadata::metadata =
    {
        "Raumbaumeister Stereo",
        "Room Builder Stereo",
        "RB1S",
        &developers::v_sadovnikov,
        "room_builder_stereo",
        "mprh",
        0,
        LSP_VERSION(1, 0, 1),
        room_builder_classes,
        E_3D_BACKEND | E_KVT_SYNC,
        room_builder_stereo_ports,
        "simulation/room_builder/stereo.xml",
        "simulation/room_builder",
        stereo_plugin_port_groups
    };
}


