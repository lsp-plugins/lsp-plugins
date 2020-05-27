/*
 * oscilloscope.cpp
 *
 *  Created on: 26 Feb 2020
 *      Author: crocoduck
 */

#include <metadata/plugins.h>
#include <metadata/ports.h>
#include <metadata/developers.h>

namespace lsp
{
    static const int osclilloscope_classes[] = { C_UTILITY, -1};

    static const port_item_t osc_trg_mode[] =
    {
        {"None", ""},
        {"Single", ""},
        {"Repeat", ""},
        NULL
    };

    static const port_item_t osc_trg_type[] =
    {
        {"Rising Edge", ""},
        {"Falling Edge", ""},
        NULL
    };

    static const port_item_t osc_coupling[] =
    {
        {"AC", ""},
        {"DC", ""},
        NULL
    };

    #define CHANNEL_AUDIO_PORTS(id, label) \
        AUDIO_INPUT("in_x" id, "Input x" label), \
        AUDIO_INPUT("in_y" id, "Input y" label), \
        AUDIO_INPUT("in_ext" id, "Input external" label), \
        AUDIO_OUTPUT("out_x" id, "Output x" label), \
        AUDIO_OUTPUT("out_y" id, "Output y" label)

    #define HOR_CONTROLS(id, label) \
        CONTROL("hzdv" id, "Horizontal Division" label, U_SEC, oscilloscope_base_metadata::TIME_DIVISION), \
        CONTROL("hzps" id, "Horizontal Position" label, U_PERCENT, oscilloscope_base_metadata::TIME_POSITION)

    #define VER_CONTROLS(id, label) \
        CONTROL("vedv" id, "Vertical Division" label, U_NONE, oscilloscope_base_metadata::VERTICAL_DIVISION), \
        CONTROL("veps" id, "Vertical Position" label, U_PERCENT, oscilloscope_base_metadata::VERTICAL_POSITION)

    #define TRG_CONTROLS(id, label) \
        CONTROL("trhy" id, "Hysteresis" label, U_NONE, oscilloscope_base_metadata::TRIGGER_HYSTERESIS), \
        CONTROL("trlv" id, "Level" label, U_PERCENT, oscilloscope_base_metadata::TRIGGER_LEVEL), \
        COMBO("trmo" id, "Mode" label, oscilloscope_base_metadata::TRIGGER_MODE_DFL, osc_trg_mode), \
        COMBO("trtp" id, "Type" label, oscilloscope_base_metadata::TRIGGER_TYPE_DFL, osc_trg_type)

    #define CPL_CONTROLS(id, label) \
        COMBO("trcp" id, "Coupling" label, oscilloscope_base_metadata::COUPLING_DFL, osc_coupling)

    #define OSC_COMMON \
        BYPASS

    #define CHANNEL_CONTROLS(id, label) \
        HOR_CONTROLS(id, label), \
        VER_CONTROLS(id, label), \
        TRG_CONTROLS(id, label), \
        CPL_CONTROLS(id, label)

    #define OSC_VISUALOUTS(id, label) \
        MESH("oscv" id, "Oscilloscope" label, 2, oscilloscope_base_metadata::SCOPE_MESH_SIZE)

    #define AUDIO_PORTS_X1 CHANNEL_AUDIO_PORTS("_1", " 1")
    #define CHANNEL_CONTROLS_X1 CHANNEL_CONTROLS("_1", " 1")
    #define OSC_VISUALOUTS_X1 OSC_VISUALOUTS("_1", " 1")

    #define AUDIO_PORTS_X2 CHANNEL_AUDIO_PORTS("_1", " 1"), CHANNEL_AUDIO_PORTS("_2", " 2")
    #define CHANNEL_CONTROLS_X2 CHANNEL_CONTROLS("_1", " 1"), CHANNEL_CONTROLS("_2", " 2")
    #define OSC_VISUALOUTS_X2 OSC_VISUALOUTS("_1", " 1"), OSC_VISUALOUTS("_2", " 2")

//    #define OSC_VISUALOUTS_X4 OSC_VISUALOUTS("_1", " 1"), OSC_VISUALOUTS("_2", " 2"), OSC_VISUALOUTS("_3", " 3"), OSC_VISUALOUTS("_4", " 4")

    static const port_t oscilloscope_x1_ports[] =
    {
        AUDIO_PORTS_X1,
        OSC_COMMON,
        CHANNEL_CONTROLS_X1,
        OSC_VISUALOUTS_X1,
        PORTS_END
    };

    static const port_t oscilloscope_x2_ports[] =
    {
        AUDIO_PORTS_X2,
        OSC_COMMON,
        CHANNEL_CONTROLS_X2,
        OSC_VISUALOUTS_X2,
        PORTS_END
    };

//    static const port_t oscilloscope_x4_ports[] =
//    {
//        PORTS_MONO_PLUGIN,
//        OSC_COMMON,
//        OSC_VISUALOUTS_X4,
//        PORTS_END
//    };

    const plugin_metadata_t oscilloscope_x1_metadata::metadata =
    {
        "Oscilloscope x1",
        "Oscilloscope x1",
        "O11", // Profiler x1 Mono
        &developers::s_tronci,
        "oscilloscope_x1",
        "",
        0,
        LSP_VERSION(1, 0, 0),
        osclilloscope_classes,
        E_NONE,
        oscilloscope_x1_ports,
        "util/oscilloscope/x1.xml",
        NULL,
        mono_plugin_port_groups
    };

    const plugin_metadata_t oscilloscope_x2_metadata::metadata =
    {
        "Oscilloscope x2",
        "Oscilloscope x2",
        "P12", // Profiler x1 Stereo
        &developers::s_tronci,
        "oscilloscope_x2",
        "",
        0,
        LSP_VERSION(1, 0, 0),
        osclilloscope_classes,
        E_NONE,
        oscilloscope_x2_ports,
        "util/oscilloscope/x2.xml",
        NULL,
        stereo_plugin_port_groups
    };
}
