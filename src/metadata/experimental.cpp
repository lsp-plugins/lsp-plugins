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

    static const char *filter_select_32ms[] =
    {
        "Mid 0-7",
        "Side 0-7",
        "Mid 8-15",
        "Side 8-15",
        "Mid 16-23",
        "Side 16-23",
        "Mid 24-31",
        "Side 24-31",
        NULL
    };

    static const char *band_selectors[] =
    {
        "0", "1", "2", "3",
        "4", "5", "6", "7",
        "8", "9", "10", "11",
        "12", "13", "14", "15",
        NULL
    };

    static const char *fb_modes[] =
    {
        "Rainbow",
        "Fog",
        "Color",
        "Lightness",
        "Lightness2",
        NULL
    };

    static const port_t test_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        AMP_GAIN10("gain", "Output gain", 1.0f),
        MESH("fftg", "Some graph", 2, 320),
        FBUFFER("fb0", "Frame buffer 0", 64, test_plugin_metadata::FRM_BUFFER_SIZE),
        COMBO("fbm0", "Frame buffer mode 0", 0, fb_modes),
        COMBO("fbm1", "Frame buffer mode 1", 0, fb_modes),
        KNOB("fbh0", "Frame buffer hue 0", U_NONE, 0.0f, 1.0f, 0.0f, 1.0f/360.0f ),
        KNOB("fbh1", "Frame buffer hue 1", U_NONE, 0.0f, 1.0f, 1.0f/3.0f, 1.0f/360.0f ),

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

    const plugin_metadata_t test_plugin_metadata::metadata =
    {
        "TEST",
        "TEST",
        "TEST",
        &developers::v_sadovnikov,
        "test_plugin",
        "TEST",
        LSP_LADSPA_BASE + 1001,
        LSP_VERSION(1, 0, 0),
        test_classes,
        test_ports,
        "experimental/test_plugin.xml",
        stereo_plugin_port_groups
    };
#endif /* LSP_NO_EXPERMIENTAL */

}



