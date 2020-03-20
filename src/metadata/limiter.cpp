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

    static port_item_t limiter_oper_modes[] =
    {
        { "Classic",        "limiter.classic" },

        { "Herm Thin",      "limiter.herm_thin" },
        { "Herm Wide",      "limiter.herm_wide" },
        { "Herm Tail",      "limiter.herm_tail" },
        { "Herm Duck",      "limiter.herm_duck" },

        { "Exp Thin",       "limiter.exp_thin" },
        { "Exp Wide",       "limiter.exp_wide" },
        { "Exp Tail",       "limiter.exp_tail" },
        { "Exp Duck",       "limiter.exp_duck" },

        { "Line Thin",      "limiter.line_thin" },
        { "Line Wide",      "limiter.line_wide" },
        { "Line Tail",      "limiter.line_tail" },
        { "Line Duck",      "limiter.line_duck" },

        { "Mixed Herm",     "limiter.mixed_herm" },
        { "Mixed Exp",      "limiter.mixed_exp" },
        { "Mixed Line",     "limiter.mixed_line" },

        { NULL, NULL }
    };

    static port_item_t limiter_ovs_modes[] =
    {
        { "None",           "oversampler.none" },

        { "Half x2(2L)",    "oversampler.half.2x2" },
        { "Half x2(3L)",    "oversampler.half.2x3" },
        { "Half x3(2L)",    "oversampler.half.3x2" },
        { "Half x3(3L)",    "oversampler.half.3x3" },
        { "Half x4(2L)",    "oversampler.half.4x2" },
        { "Half x4(3L)",    "oversampler.half.4x3" },
        { "Half x6(2L)",    "oversampler.half.6x2" },
        { "Half x6(3L)",    "oversampler.half.6x3" },
        { "Half x8(2L)",    "oversampler.half.8x2" },
        { "Half x8(3L)",    "oversampler.half.8x3" },

        { "Full x2(2L)",    "oversampler.full.2x2" },
        { "Full x2(3L)",    "oversampler.full.2x3" },
        { "Full x3(2L)",    "oversampler.full.3x2" },
        { "Full x3(3L)",    "oversampler.full.3x3" },
        { "Full x4(2L)",    "oversampler.full.4x2" },
        { "Full x4(3L)",    "oversampler.full.4x3" },
        { "Full x6(2L)",    "oversampler.full.6x2" },
        { "Full x6(3L)",    "oversampler.full.6x3" },
        { "Full x8(2L)",    "oversampler.full.8x2" },
        { "Full x8(3L)",    "oversampler.full.8x3" },

        { NULL, NULL }
    };

    static port_item_t limiter_dither_modes[] =
    {
        { "None",   "dither.none" },
        { "7bit",   "dither.bits.7" },
        { "8bit",   "dither.bits.8" },
        { "11bit",  "dither.bits.11" },
        { "12bit",  "dither.bits.12" },
        { "15bit",  "dither.bits.15" },
        { "16bit",  "dither.bits.16" },
        { "23bit",  "dither.bits.23" },
        { "24bit",  "dither.bits.24" },
        { NULL, NULL }
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
        METER_OUT_GAIN("olm" id, "Output level meter" label, GAIN_AMP_0_DB), \
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


