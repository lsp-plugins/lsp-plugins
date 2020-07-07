/*
 * surge_filter.cpp
 *
 *  Created on: 4 июл. 2020 г.
 *      Author: sadko
 */

#include <metadata/plugins.h>
#include <metadata/ports.h>
#include <metadata/developers.h>

namespace lsp
{
    static const int surge_filter_classes[] = { C_DYNAMICS, -1 };

    #define SURGE_FILTER_COMMON(channels)    \
        AMP_GAIN("input", "Input gain", 1.0f, GAIN_AMP_P_24_DB), \
        EXT_LOG_CONTROL("thresh", "Threshold", U_GAIN_AMP, surge_filter_base_metadata::THRESH), \
        LOG_CONTROL("attack", "Attack time", U_MSEC, surge_filter_base_metadata::ATTACK), \
        LOG_CONTROL("release", "Release time", U_MSEC, surge_filter_base_metadata::RELEASE), \
        CONTROL("fade", "Fade time", U_MSEC, surge_filter_base_metadata::FADE), \
        BLINK("active", "Activity indicator"), \
        AMP_GAIN("output", "Output gain", 1.0f, GAIN_AMP_P_24_DB), \
        MESH("ig", "Input signal graph", channels+1, surge_filter_base_metadata::MESH_POINTS), \
        MESH("og", "Output signal graph", channels+1, surge_filter_base_metadata::MESH_POINTS), \
        MESH("grg", "Gain reduction graph", 2, surge_filter_base_metadata::MESH_POINTS), \
        SWITCH("grv", "Gain reduction visibility", 1.0f), \
        METER_GAIN("grm", "Gain reduction meter", GAIN_AMP_P_24_DB)

    static const port_t surge_filter_mono_ports[] =
    {
        PORTS_MONO_PLUGIN,
        BYPASS,
        SURGE_FILTER_COMMON(1),
        SWITCH("igv", "Input graph visibility", 1.0f),
        SWITCH("ogv", "Output graph visibility", 1.0f),
        METER_GAIN("ilm", "Input level meter", GAIN_AMP_P_24_DB),
        METER_GAIN("olm", "Output level meter", GAIN_AMP_P_24_DB),

        PORTS_END
    };

    static const port_t surge_filter_stereo_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        BYPASS,
        SURGE_FILTER_COMMON(2),
        SWITCH("igv_l", "Input graph visibility left", 1.0f),
        SWITCH("ogv_l", "Output graph visibility left", 1.0f),
        METER_GAIN("ilm_l", "Input level meter left", GAIN_AMP_P_24_DB),
        METER_GAIN("olm_l", "Output level meter left", GAIN_AMP_P_24_DB),
        SWITCH("igv_r", "Input graph visibility right", 1.0f),
        SWITCH("ogv_r", "Output graph visibility right", 1.0f),
        METER_GAIN("ilm_r", "Input level meter right", GAIN_AMP_P_24_DB),
        METER_GAIN("olm_r", "Output level meter right", GAIN_AMP_P_24_DB),

        PORTS_END
    };

    const plugin_metadata_t surge_filter_mono_metadata::metadata =
    {
        "Sprungfilter Mono",
        "Surge Filter Mono",
        "SF1M",
        &developers::v_sadovnikov,
        "surge_filter_mono",
        "----",
        LSP_SURGE_FILTER_BASE + 0,
        LSP_VERSION(1, 0, 0),
        surge_filter_classes,
        E_INLINE_DISPLAY,
        surge_filter_mono_ports,
        "util/surge_filter.xml",
        NULL,
        mono_plugin_port_groups
    };

    const plugin_metadata_t surge_filter_stereo_metadata::metadata =
    {
        "Sprungfilter Stereo",
        "Surge Filter Stereo",
        "SF1S",
        &developers::v_sadovnikov,
        "surge_filter_stereo",
        "----",
        LSP_SURGE_FILTER_BASE + 1,
        LSP_VERSION(1, 0, 0),
        surge_filter_classes,
        E_INLINE_DISPLAY,
        surge_filter_stereo_ports,
        "util/surge_filter.xml",
        NULL,
        stereo_plugin_port_groups
    };

}


