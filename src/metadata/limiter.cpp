/*
 * limiter.cpp
 *
 *  Created on: 24 нояб. 2016 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <metadata/plugins.h>
#include <metadata/ports.h>
#include <metadata/developers.h>

namespace lsp
{
    //-------------------------------------------------------------------------
    // Limiter
    static const int limiter_classes[] = { C_LIMITER, -1 };

    static const char *limiter_oper_modes[] =
    {
        "Classic",

        "Herm Thin",
        "Herm Wide",
        "Herm Tail",
        "Herm Duck",

        "Exp Thin",
        "Exp Wide",
        "Exp Tail",
        "Exp Duck",

        "Line Thin",
        "Line Wide",
        "Line Tail",
        "Line Duck",

        "Mixed Herm",
        "Mixed Exp",
        "Mixed Line",

        NULL
    };

    static const char *limiter_ovs_modes[] =
    {
        "None",
        "Half x2(2L)",
        "Half x2(3L)",
        "Half x3(2L)",
        "Half x3(3L)",
        "Half x4(2L)",
        "Half x4(3L)",
        "Half x6(2L)",
        "Half x6(3L)",
        "Half x8(2L)",
        "Half x8(3L)",
        "Full x2(2L)",
        "Full x2(3L)",
        "Full x3(2L)",
        "Full x3(3L)",
        "Full x4(2L)",
        "Full x4(3L)",
        "Full x6(2L)",
        "Full x6(3L)",
        "Full x8(2L)",
        "Full x8(3L)",
        NULL
    };

    static const char *limiter_dither_modes[] =
    {
        "None",
        "7bit",
        "8bit",
        "11bit",
        "12bit",
        "15bit",
        "16bit",
        "23bit",
        "24bit",
        NULL
    };

    #define LIMIT_COMMON    \
        BYPASS,             \
        IN_GAIN,            \
        OUT_GAIN,           \
        AMP_GAIN100("scp", "Sidechain preamp", GAIN_AMP_0_DB), \
        COMBO("mode", "Operating mode", limiter_base_metadata::LOM_DEFAULT, limiter_oper_modes), \
        LOG_CONTROL("th", "Threshold", U_GAIN_AMP, limiter_base_metadata::THRESHOLD), \
        LOG_CONTROL("knee", "Knee", U_GAIN_AMP, limiter_base_metadata::KNEE), \
        SWITCH("boost", "Gain boost", 1.0f), \
        LOG_CONTROL("lk", "Lookahead", U_MSEC, limiter_base_metadata::LOOKAHEAD), \
        LOG_CONTROL("at", "Attack time", U_MSEC, limiter_base_metadata::ATTACK_TIME), \
        LOG_CONTROL("rt", "Release time", U_MSEC, limiter_base_metadata::RELEASE_TIME), \
        COMBO("ovs", "Oversampling", limiter_base_metadata::OVS_DEFAULT, limiter_ovs_modes),           \
        COMBO("dith", "Dithering", limiter_base_metadata::DITHER_DEFAULT, limiter_dither_modes),           \
        SWITCH("pause", "Pause graph analysis", 0.0f), \
        TRIGGER("clear", "Clear graph analysis")

    #define LIMIT_COMMON_SC \
        SWITCH("extsc", "External sidechain", 0.0f)

    #define LIMIT_COMMON_MONO       \
        LIMIT_COMMON
    #define LIMIT_COMMON_STEREO     \
        LIMIT_COMMON_MONO, \
        LOG_CONTROL("slink", "Stereo linking", U_PERCENT, limiter_base_metadata::LINKING)

    #define LIMIT_COMMON_SC_MONO    \
        LIMIT_COMMON, \
        LIMIT_COMMON_SC

    #define LIMIT_COMMON_SC_STEREO  \
        LIMIT_COMMON_STEREO, \
        LIMIT_COMMON_SC

    #define LIMIT_METERS(id, label) \
        SWITCH("igv" id, "Input graph visibility" label, 1.0f), \
        SWITCH("ogv" id, "Output graph visibility" label, 1.0f), \
        SWITCH("scgv" id, "Sidechain graph visibility" label, 1.0f), \
        SWITCH("grgv" id, "Gain graph visibility" label, 1.0f), \
        METER_OUT_GAIN("ilm" id, "Input level meter" label, GAIN_AMP_0_DB), \
        METER_OUT_GAIN("olm" id, "Outut level meter" label, GAIN_AMP_0_DB), \
        METER_OUT_GAIN("sclm" id, "Sidechain level meter" label, GAIN_AMP_0_DB), \
        METER_GAIN_DFL("grlm" id, "Gain reduction level meter" label, GAIN_AMP_0_DB, GAIN_AMP_0_DB), \
        MESH("ig" id, "Input graph" label, 2, limiter_base_metadata::HISTORY_MESH_SIZE), \
        MESH("og" id, "Output graph" label, 2, limiter_base_metadata::HISTORY_MESH_SIZE), \
        MESH("scg" id, "Sidechain graph" label, 2, limiter_base_metadata::HISTORY_MESH_SIZE), \
        MESH("grg" id, "Gain graph" label, 2, limiter_base_metadata::HISTORY_MESH_SIZE)

    #define LIMIT_METERS_MONO       LIMIT_METERS("", "")
    #define LIMIT_METERS_STEREO     LIMIT_METERS("_l", " Left"), LIMIT_METERS("_r", " Right")

    static const port_t limiter_mono_ports[] =
    {
        PORTS_MONO_PLUGIN,
        LIMIT_COMMON_MONO,
        LIMIT_METERS_MONO,

        PORTS_END
    };

    static const port_t limiter_stereo_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        LIMIT_COMMON_STEREO,
        LIMIT_METERS_STEREO,

        PORTS_END
    };

    static const port_t sc_limiter_mono_ports[] =
    {
        PORTS_MONO_PLUGIN,
        PORTS_MONO_SIDECHAIN,
        LIMIT_COMMON_SC_MONO,
        LIMIT_METERS_MONO,

        PORTS_END
    };

    static const port_t sc_limiter_stereo_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        PORTS_STEREO_SIDECHAIN,
        LIMIT_COMMON_SC_STEREO,
        LIMIT_METERS_STEREO,

        PORTS_END
    };

    // Limiter
    const plugin_metadata_t  limiter_mono_metadata::metadata =
    {
        "Begrenzer Mono",
        "Limiter Mono",
        "B1M",
        &developers::v_sadovnikov,
        "limiter_mono",
        "jz5z",
        LSP_LIMITER_BASE + 0,
        LSP_VERSION(1, 0, 1),
        limiter_classes,
        E_INLINE_DISPLAY,
        limiter_mono_ports,
        "dynamics/limiter/single/mono.xml",
        NULL,
        mono_plugin_port_groups
    };

    const plugin_metadata_t  limiter_stereo_metadata::metadata =
    {
        "Begrenzer Stereo",
        "Limiter Stereo",
        "B1S",
        &developers::v_sadovnikov,
        "limiter_stereo",
        "rfuc",
        LSP_LIMITER_BASE + 1,
        LSP_VERSION(1, 0, 1),
        limiter_classes,
        E_INLINE_DISPLAY,
        limiter_stereo_ports,
        "dynamics/limiter/single/stereo.xml",
        NULL,
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  sc_limiter_mono_metadata::metadata =
    {
        "Sidechain-Begrenzer Mono",
        "Sidechain Limiter Mono",
        "SCB1M",
        &developers::v_sadovnikov,
        "sc_limiter_mono",
        "kyzu",
        LSP_LIMITER_BASE + 2,
        LSP_VERSION(1, 0, 1),
        limiter_classes,
        E_INLINE_DISPLAY,
        sc_limiter_mono_ports,
        "dynamics/limiter/single/mono.xml",
        NULL,
        mono_plugin_sidechain_port_groups
    };

    const plugin_metadata_t  sc_limiter_stereo_metadata::metadata =
    {
        "Sidechain-Begrenzer Stereo",
        "Sidechain Limiter Stereo",
        "SCB1S",
        &developers::v_sadovnikov,
        "sc_limiter_stereo",
        "zwf7",
        LSP_LIMITER_BASE + 3,
        LSP_VERSION(1, 0, 1),
        limiter_classes,
        E_INLINE_DISPLAY,
        sc_limiter_stereo_ports,
        "dynamics/limiter/single/stereo.xml",
        NULL,
        stereo_plugin_sidechain_port_groups
    };
}


