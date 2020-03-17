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
#ifndef LSP_NO_EXPERIMENTAL
    static const int test_classes[] = { -1 };

    #define TEST_EQ_BAND(id) \
        LOG_CONTROL("g_" #id, "Band gain " #id, U_GAIN_AMP, test_plugin_metadata::BAND_GAIN)

    static const port_item_t filter_select_32ms[] =
    {
        { "Mid 0-7", NULL },
        { "Side 0-7", NULL },
        { "Mid 8-15", NULL },
        { "Side 8-15", NULL },
        { "Mid 16-23", NULL },
        { "Side 16-23", NULL },
        { "Mid 24-31", NULL },
        { "Side 24-31", NULL },
        { NULL, NULL }
    };

    static const port_item_t band_selectors[] =
    {
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

    static const port_item_t fb_modes[] =
    {
        { "Rainbow", NULL },
        { "Fog", NULL },
        { "Color", NULL },
        { "Lightness", NULL },
        { "Lightness2", NULL },
        { NULL, NULL }
    };

    #define FT(x) { #x, NULL }

    static const port_item_t filter_types[] =
    {
        FT(FLT_NONE),

        FT(FLT_BT_AMPLIFIER),
        FT(FLT_MT_AMPLIFIER),

        FT(FLT_BT_RLC_LOPASS),
        FT(FLT_MT_RLC_LOPASS),
        FT(FLT_BT_RLC_HIPASS),
        FT(FLT_MT_RLC_HIPASS),
        FT(FLT_BT_RLC_LOSHELF),
        FT(FLT_MT_RLC_LOSHELF),
        FT(FLT_BT_RLC_HISHELF),
        FT(FLT_MT_RLC_HISHELF),
        FT(FLT_BT_RLC_BELL),
        FT(FLT_MT_RLC_BELL),
        FT(FLT_BT_RLC_RESONANCE),
        FT(FLT_MT_RLC_RESONANCE),
        FT(FLT_BT_RLC_NOTCH),
        FT(FLT_MT_RLC_NOTCH),
        FT(FLT_BT_RLC_ALLPASS),
        FT(FLT_MT_RLC_ALLPASS),
        FT(FLT_BT_RLC_ALLPASS2),
        FT(FLT_MT_RLC_ALLPASS2),
        FT(FLT_BT_RLC_LADDERPASS),
        FT(FLT_MT_RLC_LADDERPASS),
        FT(FLT_BT_RLC_LADDERREJ),
        FT(FLT_MT_RLC_LADDERREJ),
        FT(FLT_BT_RLC_BANDPASS),
        FT(FLT_MT_RLC_BANDPASS),
        FT(FLT_BT_RLC_ENVELOPE),
        FT(FLT_MT_RLC_ENVELOPE),

        FT(FLT_BT_BWC_LOPASS),
        FT(FLT_MT_BWC_LOPASS),
        FT(FLT_BT_BWC_HIPASS),
        FT(FLT_MT_BWC_HIPASS),
        FT(FLT_BT_BWC_LOSHELF),
        FT(FLT_MT_BWC_LOSHELF),
        FT(FLT_BT_BWC_HISHELF),
        FT(FLT_MT_BWC_HISHELF),
        FT(FLT_BT_BWC_BELL),
        FT(FLT_MT_BWC_BELL),
        FT(FLT_BT_BWC_LADDERPASS),
        FT(FLT_MT_BWC_LADDERPASS),
        FT(FLT_BT_BWC_LADDERREJ),
        FT(FLT_MT_BWC_LADDERREJ),
        FT(FLT_BT_BWC_BANDPASS),
        FT(FLT_MT_BWC_BANDPASS),
        FT(FLT_BT_BWC_ALLPASS),
        FT(FLT_MT_BWC_ALLPASS),

        FT(FLT_BT_LRX_LOPASS),
        FT(FLT_MT_LRX_LOPASS),
        FT(FLT_BT_LRX_HIPASS),
        FT(FLT_MT_LRX_HIPASS),
        FT(FLT_BT_LRX_LOSHELF),
        FT(FLT_MT_LRX_LOSHELF),
        FT(FLT_BT_LRX_HISHELF),
        FT(FLT_MT_LRX_HISHELF),
        FT(FLT_BT_LRX_BELL),
        FT(FLT_MT_LRX_BELL),
        FT(FLT_BT_LRX_LADDERPASS),
        FT(FLT_MT_LRX_LADDERPASS),
        FT(FLT_BT_LRX_LADDERREJ),
        FT(FLT_MT_LRX_LADDERREJ),
        FT(FLT_BT_LRX_BANDPASS),
        FT(FLT_MT_LRX_BANDPASS),
        FT(FLT_BT_LRX_ALLPASS),
        FT(FLT_MT_LRX_ALLPASS),

        FT(FLT_DR_APO_LOPASS),
        FT(FLT_DR_APO_HIPASS),
        FT(FLT_DR_APO_BANDPASS),
        FT(FLT_DR_APO_NOTCH),
        FT(FLT_DR_APO_ALLPASS),
        FT(FLT_DR_APO_ALLPASS2),
        FT(FLT_DR_APO_PEAKING),
        FT(FLT_DR_APO_LOSHELF),
        FT(FLT_DR_APO_HISHELF),
        FT(FLT_DR_APO_LADDERPASS),
        FT(FLT_DR_APO_LADDERREJ),

        { NULL, NULL }
    };

    static const port_item_t filter_slopes[] =
    {
        { "x1", NULL },
        { "x2", NULL },
        { "x3", NULL },
        { "x4", NULL },
        { "x5", NULL },
        { "x6", NULL },
        { "x7", NULL },
        { "x8", NULL },
        { NULL, NULL }
    };

    static const port_item_t filter_ops[] =
    {
        { "none", NULL },
        { "add", NULL },
        { "sub", NULL },
        { NULL, NULL }
    };

    static const port_item_t filter_graphs[] =
    {
        { "Amplitude", NULL },
        { "Phase", NULL },
        { NULL, NULL }
    };

    static const port_t test_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        AMP_GAIN10("gain", "Output gain", 1.0f),
        MESH("fftg", "Some graph", 2, 320),
        FBUFFER("fb0", "Frame buffer 0", 64, test_plugin_metadata::FRM_BUFFER_SIZE),
        COMBO("fbm0", "Frame buffer mode 0", 0, fb_modes),
        COMBO("fbm1", "Frame buffer mode 1", 0, fb_modes),
        CKNOB("fbh0", "Frame buffer hue 0", U_NONE, 0.0f, 1.0f, 0.0f, 1.0f/360.0f ),
        CKNOB("fbh1", "Frame buffer hue 1", U_NONE, 0.0f, 1.0f, 1.0f/3.0f, 1.0f/360.0f ),

        PATH("ifn", "File name"),
        CONTROL("ihc", "Head cut", U_MSEC, test_plugin_metadata::FILE_LENGTH),
        CONTROL("itc", "Tail cut", U_MSEC, test_plugin_metadata::FILE_LENGTH),
        CONTROL("ifi", "Fade in", U_MSEC, test_plugin_metadata::FILE_LENGTH),
        CONTROL("ifo", "Fade out", U_MSEC, test_plugin_metadata::FILE_LENGTH),
        STATUS("ifs", "Load status"),
        METER("ifl", "File length", U_MSEC, test_plugin_metadata::FILE_LENGTH),
        MESH("ifd", "File contents", test_plugin_metadata::TRACKS_MAX, test_plugin_metadata::MESH_SIZE),

        PATH("ofn", "Save file name"),
        TRIGGER("ofc", "Save file command"),
        STATUS("ofs", "File saving status"),
        METER_PERCENT("ofp", "File saving progress"),

        SWITCH("sw0", "Switch 0", 0.0f),
        TRIGGER("sw1", "Switch 1"),
        SWITCH("sw2", "Switch 2", 0.0f),
        SWITCH("sw3", "Switch 3", 0.0f),
        AMP_GAIN10("kn0", "Knob 0", 1.0f),
        AMP_GAIN100("kn1", "Knob 1", 1.0f),
        KNOB("kn2", "Knob 2", U_MSEC, 0, 1000, 500, 0.01),
        { "kn3", "Knob 3", U_M, R_CONTROL, F_IN | F_INT | F_UPPER | F_LOWER | F_STEP, 0, 200, 0, 1, NULL },

        AMP_GAIN10("fd0", "Fader 0", 1.0f),
        AMP_GAIN100("fd1", "Fader 1", 1.0f),
        KNOB("fd2", "Fader 2", U_MSEC, 0, 1000, 500, 0.01),
        { "fd3", "Fader 3", U_M, R_CONTROL, F_IN | F_INT | F_UPPER | F_LOWER | F_STEP, 0, 200, 0, 1, NULL },

        AMP_GAIN10("imk", "File makeup gain", 1.0f),
        CONTROL("ipd", "Sample pre-delay", U_MSEC, test_plugin_metadata::PREDELAY),
        TRIGGER("ils", "Sample listen"),

        TEST_EQ_BAND(0),
        TEST_EQ_BAND(1),
        TEST_EQ_BAND(2),
        TEST_EQ_BAND(3),
        TEST_EQ_BAND(4),
        TEST_EQ_BAND(5),
        TEST_EQ_BAND(6),
        TEST_EQ_BAND(7),
        TEST_EQ_BAND(8),
        TEST_EQ_BAND(9),
        TEST_EQ_BAND(10),
        TEST_EQ_BAND(11),
        TEST_EQ_BAND(12),
        TEST_EQ_BAND(13),
        TEST_EQ_BAND(14),
        TEST_EQ_BAND(15),

        COMBO("fsel", "Filter select", 0, filter_select_32ms),
        COMBO("bsel", "Band select", 0, band_selectors),

        PORTS_END
    };

    #define FLT_MONO_PORTS(id) \
        COMBO("ft" id, "Filter type " id, 0, filter_types), \
        COMBO("s" id, "Filter slope " id, 0, filter_slopes), \
        COMBO("op" id, "Filter operation " id, 0, filter_ops), \
        LOG_CONTROL_DFL("fl" id, "Low frequency " id, U_HZ, filter_analyzer_metadata::FREQ, filter_analyzer_metadata::FREQ_DFL), \
        LOG_CONTROL_DFL("fh" id, "High frequency " id, U_HZ, filter_analyzer_metadata::FREQ, filter_analyzer_metadata::FREQ_DFL * 10), \
        LOG_CONTROL("g" id, "Gain " id, U_GAIN_AMP, filter_analyzer_metadata::GAIN), \
        CONTROL("q" id, "Quality factor " id, U_NONE, filter_analyzer_metadata::QUALITY)

    static const port_t filter_analyzer_ports[] =
    {
        PORTS_MONO_PLUGIN,
        COMBO("area", "Filter graph", 0, filter_graphs),
        MESH("fg", "Filter graph", 3, filter_analyzer_metadata::MESH_POINTS),
        FLT_MONO_PORTS("0"),
        FLT_MONO_PORTS("1"),
        PORTS_END
    };

    const plugin_metadata_t test_plugin_metadata::metadata =
    {
        "TEST",
        "TEST",
        "TEST",
        &developers::v_sadovnikov,
        "test_plugin",
        "TEST",
        LSP_LADSPA_BASE + 4001,
        LSP_VERSION(1, 0, 0),
        test_classes,
        E_INLINE_DISPLAY,
        test_ports,
        "experimental/test_plugin.xml",
        NULL,
        stereo_plugin_port_groups
    };

    const plugin_metadata_t filter_analyzer_metadata::metadata =
    {
        "Filter analyzer",
        "Filter analyzer",
        "FA2",
        &developers::v_sadovnikov,
        "filter_analyzer",
        "xFA2",
        LSP_LADSPA_BASE + 4002,
        LSP_VERSION(1, 0, 0),
        test_classes,
        E_NONE,
        filter_analyzer_ports,
        "experimental/filter_analyzer.xml",
        NULL,
        mono_plugin_port_groups
    };
#endif /* LSP_NO_EXPERMIENTAL */

}



