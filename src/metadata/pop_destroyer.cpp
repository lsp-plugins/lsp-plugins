/*
 * pop_destroyer.cpp
 *
 *  Created on: 4 июл. 2020 г.
 *      Author: sadko
 */

#include <metadata/plugins.h>
#include <metadata/ports.h>
#include <metadata/developers.h>

namespace lsp
{
    static const int pop_destroyer_classes[] = { C_DYNAMICS, -1 };

    #define POP_DESTROYER_COMMON(channels)    \
        AMP_GAIN("input", "Input gain", 0.0f, GAIN_AMP_P_24_DB), \
        EXT_LOG_CONTROL("thresh", "Threshold", U_GAIN_AMP, pop_destroyer_base_metadata::THRESH), \
        LOG_CONTROL("attack", "Attack time", U_MSEC, pop_destroyer_base_metadata::ATTACK), \
        LOG_CONTROL("release", "Release time", U_MSEC, pop_destroyer_base_metadata::RELEASE), \
        CONTROL("fade", "Fade time", U_MSEC, pop_destroyer_base_metadata::FADE), \
        BLINK("active", "Activity indicator"), \
        AMP_GAIN("output", "Output gain", 0.0f, GAIN_AMP_P_24_DB), \
        MESH("ing", "Input signal graph", channels+1, pop_destroyer_base_metadata::MESH_POINTS), \
        MESH("outg", "Output signal graph", channels+1, pop_destroyer_base_metadata::MESH_POINTS), \
        MESH("gaing", "Gain graph", channels, pop_destroyer_base_metadata::MESH_POINTS), \
        SWITCH("gains", "Gain graph show", 1.0f)

    static const port_t pop_destroyer_mono_ports[] =
    {
        PORTS_MONO_PLUGIN,
        BYPASS,
        POP_DESTROYER_COMMON(1),
        SWITCH("ins", "Input graph show", 1.0f),
        SWITCH("outs", "Output graph show", 1.0f),
        METER_GAIN("ilm", "Input level meter", GAIN_AMP_P_24_DB),
        METER_GAIN("olm", "Output level meter", GAIN_AMP_P_24_DB),

        PORTS_END
    };

    static const port_t pop_destroyer_stereo_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        BYPASS,
        POP_DESTROYER_COMMON(2),
        SWITCH("ins_l", "Input graph show left", 1.0f),
        SWITCH("outs_l", "Output graph show left", 1.0f),
        METER_GAIN("ilm_l", "Input level meter left", GAIN_AMP_P_24_DB),
        METER_GAIN("olm_l", "Output level meter left", GAIN_AMP_P_24_DB),
        SWITCH("ins_r", "Input graph show right", 1.0f),
        SWITCH("outs_r", "Output graph show right", 1.0f),
        METER_GAIN("ilm_r", "Input level meter right", GAIN_AMP_P_24_DB),
        METER_GAIN("olm_r", "Output level meter right", GAIN_AMP_P_24_DB),

        PORTS_END
    };

    const plugin_metadata_t pop_destroyer_mono_metadata::metadata =
    {
        "Pop Zerstörer Mono",
        "Pop Destroyer Mono",
        "PZ1M",
        &developers::v_sadovnikov,
        "pop_destroyer_mono",
        "----",
        LSP_POP_DESTROYER_BASE + 0,
        LSP_VERSION(1, 0, 0),
        pop_destroyer_classes,
        E_INLINE_DISPLAY,
        pop_destroyer_mono_ports,
        "util/pop_destroyer.xml",
        NULL,
        mono_plugin_port_groups
    };

    const plugin_metadata_t pop_destroyer_stereo_metadata::metadata =
    {
        "Pop Zerstörer Stereo",
        "Pop Destroyer Stereo",
        "PZ1S",
        &developers::v_sadovnikov,
        "pop_destroyer_stereo",
        "----",
        LSP_POP_DESTROYER_BASE + 1,
        LSP_VERSION(1, 0, 0),
        pop_destroyer_classes,
        E_INLINE_DISPLAY,
        pop_destroyer_stereo_ports,
        "util/pop_destroyer.xml",
        NULL,
        stereo_plugin_port_groups
    };

}


