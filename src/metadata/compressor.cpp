/*
 * compressor.cpp
 *
 *  Created on: 15 сен. 2016 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <metadata/plugins.h>
#include <metadata/ports.h>
#include <metadata/developers.h>

namespace lsp
{
    //-------------------------------------------------------------------------
    // Compressor
    static const int compressor_classes[] = { C_COMPRESSOR, -1 };

    static const port_item_t comp_sc_modes[] =
    {
        { "Peak",       "sidechain.peak"           },
        { "RMS",        "sidechain.rms"            },
        { "Low-Pass",   "sidechain.lowpass"        },
        { "Uniform",    "sidechain.uniform"        },
        { NULL, NULL }
    };

    static const port_item_t comp_sc_sources[] =
    {
        { "Middle",     "sidechain.middle" },
        { "Side",       "sidechain.side" },
        { "Left",       "sidechain.left" },
        { "Right",      "sidechain.right" },
        { NULL, NULL }
    };

    static const port_item_t comp_sc_type[] =
    {
        { "Feed-forward",   "sidechain.feed_forward" },
        { "Feed-back",      "sidechain.feed_back" },
        { NULL, NULL }
    };

    static const port_item_t comp_sc2_type[] =
    {
        { "Feed-forward",   "sidechain.feed_forward" },
        { "Feed-back",      "sidechain.feed_back" },
        { "External",       "sidechain.external" },
        { NULL, NULL }
    };

    static const port_item_t comp_modes[] =
    {
        { "Downward",   "compressor.downward" },
        { "Upward",     "compressor.upward" },
        { NULL, NULL }
    };

    static const port_item_t comp_modes_short[] =
    {
        { "Down",       "compressor.down_ward" },
        { "Up",         "compressor.up_ward" },
        { NULL, NULL }
    };

    #define COMP_COMMON     \
        BYPASS,             \
        IN_GAIN,            \
        OUT_GAIN,           \
        SWITCH("pause", "Pause graph analysis", 0.0f), \
        TRIGGER("clear", "Clear graph analysis")

    #define COMP_MS_COMMON  \
        COMP_COMMON,        \
        SWITCH("msl", "Mid/Side listen", 0.0f)

    #define COMP_SC_MONO_CHANNEL(sct) \
        COMBO("sct", "Sidechain type", compressor_base_metadata::SC_TYPE_DFL, sct), \
        COMBO("scm", "Sidechain mode", compressor_base_metadata::SC_MODE_DFL, comp_sc_modes), \
        CONTROL("sla", "Sidechain lookahead", U_MSEC, compressor_base_metadata::LOOKAHEAD), \
        SWITCH("scl", "Sidechain listen", 0.0f), \
        LOG_CONTROL("scr", "Sidechain reactivity", U_MSEC, compressor_base_metadata::REACTIVITY), \
        AMP_GAIN100("scp", "Sidechain preamp", GAIN_AMP_0_DB)

    #define COMP_SC_STEREO_CHANNEL(id, label, sct) \
        COMBO("sct" id, "Sidechain type" label, compressor_base_metadata::SC_TYPE_DFL, sct), \
        COMBO("scm" id, "Sidechain mode" label, compressor_base_metadata::SC_MODE_DFL, comp_sc_modes), \
        CONTROL("sla" id, "Sidechain lookahead" label, U_MSEC, compressor_base_metadata::LOOKAHEAD), \
        SWITCH("scl" id, "Sidechain listen" label, 0.0f), \
        COMBO("scs" id, "Sidechain source" label, compressor_base_metadata::SC_SOURCE_DFL, comp_sc_sources), \
        LOG_CONTROL("scr" id, "Sidechain reactivity" label, U_MSEC, compressor_base_metadata::REACTIVITY), \
        AMP_GAIN100("scp" id, "Sidechain preamp" label, GAIN_AMP_0_DB)

    #define COMP_CHANNEL(id, label, modes) \
        COMBO("cm" id, "Compression mode" label, compressor_base_metadata::CM_DEFAULT, modes), \
        LOG_CONTROL("al" id, "Attack level" label, U_GAIN_AMP, compressor_base_metadata::ATTACK_LVL), \
        LOG_CONTROL("at" id, "Attack time" label, U_MSEC, compressor_base_metadata::ATTACK_TIME), \
        LOG_CONTROL("rrl" id, "Relative release level" label, U_GAIN_AMP, compressor_base_metadata::RELEASE_LVL), \
        LOG_CONTROL("rt" id, "Release time" label, U_MSEC, compressor_base_metadata::RELEASE_TIME), \
        LOG_CONTROL("cr" id, "Ratio" label, U_NONE, compressor_base_metadata::RATIO), \
        LOG_CONTROL("kn" id, "Knee" label, U_GAIN_AMP, compressor_base_metadata::KNEE), \
        EXT_LOG_CONTROL("bth" id, "Boost threshold" label, U_GAIN_AMP, compressor_base_metadata::BTH), \
        LOG_CONTROL("mk" id, "Makeup gain" label, U_GAIN_AMP, compressor_base_metadata::MAKEUP), \
        AMP_GAIN10("cdr" id, "Dry gain" label, GAIN_AMP_M_INF_DB),     \
        AMP_GAIN10("cwt" id, "Wet gain" label, GAIN_AMP_0_DB), \
        METER_OUT_GAIN("rl" id, "Release level" label, 20.0f), \
        SWITCH("slv" id, "Sidechain level visibility" label, 1.0f), \
        SWITCH("elv" id, "Envelope level visibility" label, 1.0f), \
        SWITCH("grv" id, "Gain reduction visibility" label, 1.0f), \
        MESH("ccg" id, "Compressor curve graph" label, 2, compressor_base_metadata::CURVE_MESH_SIZE), \
        MESH("scg" id, "Compressor sidechain graph" label, 2, compressor_base_metadata::TIME_MESH_SIZE), \
        MESH("evg" id, "Compressor envelope graph" label, 2, compressor_base_metadata::TIME_MESH_SIZE), \
        MESH("grg" id, "Compressor gain reduciton" label, 2, compressor_base_metadata::TIME_MESH_SIZE), \
        METER_OUT_GAIN("slm" id, "Sidechain level meter" label, GAIN_AMP_P_24_DB), \
        METER_OUT_GAIN("clm" id, "Curve level meter" label, GAIN_AMP_P_24_DB), \
        METER_OUT_GAIN("elm" id, "Envelope level meter" label, GAIN_AMP_P_24_DB), \
        METER_GAIN_DFL("rlm" id, "Reduction level meter" label, GAIN_AMP_P_48_DB, GAIN_AMP_0_DB)

    #define COMP_AUDIO_METER(id, label) \
        SWITCH("ilv" id, "Input level visibility" label, 1.0f), \
        SWITCH("olv" id, "Output level visibility" label, 1.0f), \
        MESH("icg" id, "Compressor input" label, 2, compressor_base_metadata::TIME_MESH_SIZE), \
        MESH("ocg" id, "Compressor output" label, 2, compressor_base_metadata::TIME_MESH_SIZE), \
        METER_GAIN("ilm" id, "Input level meter" label, GAIN_AMP_P_12_DB), \
        METER_GAIN("olm" id, "Output level meter" label, GAIN_AMP_P_12_DB)

    static const port_t compressor_mono_ports[] =
    {
        PORTS_MONO_PLUGIN,
        COMP_COMMON,
        COMP_SC_MONO_CHANNEL(comp_sc_type),
        COMP_CHANNEL("", "", comp_modes),
        COMP_AUDIO_METER("", ""),

        PORTS_END
    };

    static const port_t compressor_stereo_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        COMP_COMMON,
        COMP_SC_STEREO_CHANNEL("", "", comp_sc_type),
        COMP_CHANNEL("", "", comp_modes),
        COMP_AUDIO_METER("_l", " Left"),
        COMP_AUDIO_METER("_r", " Right"),

        PORTS_END
    };

    static const port_t compressor_lr_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        COMP_COMMON,
        COMP_SC_STEREO_CHANNEL("_l", " Left", comp_sc_type),
        COMP_SC_STEREO_CHANNEL("_r", " Right", comp_sc_type),
        COMP_CHANNEL("_l", " Left", comp_modes_short),
        COMP_CHANNEL("_r", " Right", comp_modes_short),
        COMP_AUDIO_METER("_l", " Left"),
        COMP_AUDIO_METER("_r", " Right"),

        PORTS_END
    };

    static const port_t compressor_ms_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        COMP_MS_COMMON,
        COMP_SC_STEREO_CHANNEL("_m", " Mid", comp_sc_type),
        COMP_SC_STEREO_CHANNEL("_s", " Side", comp_sc_type),
        COMP_CHANNEL("_m", " Mid", comp_modes_short),
        COMP_CHANNEL("_s", " Side", comp_modes_short),
        COMP_AUDIO_METER("_m", " Mid"),
        COMP_AUDIO_METER("_s", " Side"),

        PORTS_END
    };

    static const port_t sc_compressor_mono_ports[] =
    {
        PORTS_MONO_PLUGIN,
        PORTS_MONO_SIDECHAIN,
        COMP_COMMON,
        COMP_SC_MONO_CHANNEL(comp_sc2_type),
        COMP_CHANNEL("", "", comp_modes),
        COMP_AUDIO_METER("", ""),

        PORTS_END
    };

    static const port_t sc_compressor_stereo_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        PORTS_STEREO_SIDECHAIN,
        COMP_COMMON,
        COMP_SC_STEREO_CHANNEL("", "", comp_sc2_type),
        COMP_CHANNEL("", "", comp_modes),
        COMP_AUDIO_METER("_l", " Left"),
        COMP_AUDIO_METER("_r", " Right"),

        PORTS_END
    };

    static const port_t sc_compressor_lr_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        PORTS_STEREO_SIDECHAIN,
        COMP_COMMON,
        COMP_SC_STEREO_CHANNEL("_l", " Left", comp_sc2_type),
        COMP_SC_STEREO_CHANNEL("_r", " Right", comp_sc2_type),
        COMP_CHANNEL("_l", " Left", comp_modes_short),
        COMP_CHANNEL("_r", " Right", comp_modes_short),
        COMP_AUDIO_METER("_l", " Left"),
        COMP_AUDIO_METER("_r", " Right"),

        PORTS_END
    };

    static const port_t sc_compressor_ms_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        PORTS_STEREO_SIDECHAIN,
        COMP_MS_COMMON,
        COMP_SC_STEREO_CHANNEL("_m", " Mid", comp_sc2_type),
        COMP_SC_STEREO_CHANNEL("_s", " Side", comp_sc2_type),
        COMP_CHANNEL("_m", " Mid", comp_modes_short),
        COMP_CHANNEL("_s", " Side", comp_modes_short),
        COMP_AUDIO_METER("_m", " Mid"),
        COMP_AUDIO_METER("_s", " Side"),

        PORTS_END
    };

    // Compressor
    const plugin_metadata_t  compressor_mono_metadata::metadata =
    {
        "Kompressor Mono",
        "Compressor Mono",
        "K1M",
        &developers::v_sadovnikov,
        "compressor_mono",
        "bgsy",
        LSP_COMPRESSOR_BASE + 0,
        LSP_VERSION(1, 0, 1),
        compressor_classes,
        E_INLINE_DISPLAY,
        compressor_mono_ports,
        "dynamics/compressor/single/mono.xml",
        NULL,
        mono_plugin_port_groups
    };

    const plugin_metadata_t  compressor_stereo_metadata::metadata =
    {
        "Kompressor Stereo",
        "Compressor Stereo",
        "K1S",
        &developers::v_sadovnikov,
        "compressor_stereo",
        "unsc",
        LSP_COMPRESSOR_BASE + 1,
        LSP_VERSION(1, 0, 1),
        compressor_classes,
        E_INLINE_DISPLAY,
        compressor_stereo_ports,
        "dynamics/compressor/single/stereo.xml",
        NULL,
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  compressor_lr_metadata::metadata =
    {
        "Kompressor LeftRight",
        "Compressor LeftRight",
        "K1LR",
        &developers::v_sadovnikov,
        "compressor_lr",
        "3nam",
        LSP_COMPRESSOR_BASE + 2,
        LSP_VERSION(1, 0, 1),
        compressor_classes,
        E_INLINE_DISPLAY,
        compressor_lr_ports,
        "dynamics/compressor/single/lr.xml",
        NULL,
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  compressor_ms_metadata::metadata =
    {
        "Kompressor MidSide",
        "Compressor MidSide",
        "K1MS",
        &developers::v_sadovnikov,
        "compressor_ms",
        "jjef",
        LSP_COMPRESSOR_BASE + 3,
        LSP_VERSION(1, 0, 1),
        compressor_classes,
        E_INLINE_DISPLAY,
        compressor_ms_ports,
        "dynamics/compressor/single/ms.xml",
        NULL,
        stereo_plugin_port_groups
    };

    // Sidechain compressor
    const plugin_metadata_t  sc_compressor_mono_metadata::metadata =
    {
        "Sidechain-Kompressor Mono",
        "Sidechain Compressor Mono",
        "SCK1M",
        &developers::v_sadovnikov,
        "sc_compressor_mono",
        "lyjq",
        LSP_COMPRESSOR_BASE + 4,
        LSP_VERSION(1, 0, 1),
        compressor_classes,
        E_INLINE_DISPLAY,
        sc_compressor_mono_ports,
        "dynamics/compressor/single/mono.xml",
        NULL,
        mono_plugin_sidechain_port_groups
    };

    const plugin_metadata_t  sc_compressor_stereo_metadata::metadata =
    {
        "Sidechain-Kompressor Stereo",
        "Sidechain Compressor Stereo",
        "SCK1S",
        &developers::v_sadovnikov,
        "sc_compressor_stereo",
        "5xzi",
        LSP_COMPRESSOR_BASE + 5,
        LSP_VERSION(1, 0, 1),
        compressor_classes,
        E_INLINE_DISPLAY,
        sc_compressor_stereo_ports,
        "dynamics/compressor/single/stereo.xml",
        NULL,
        stereo_plugin_sidechain_port_groups
    };

    const plugin_metadata_t  sc_compressor_lr_metadata::metadata =
    {
        "Sidechain-Kompressor LeftRight",
        "Sidechain Compressor LeftRight",
        "SCK1LR",
        &developers::v_sadovnikov,
        "sc_compressor_lr",
        "fowg",
        LSP_COMPRESSOR_BASE + 6,
        LSP_VERSION(1, 0, 1),
        compressor_classes,
        E_INLINE_DISPLAY,
        sc_compressor_lr_ports,
        "dynamics/compressor/single/lr.xml",
        NULL,
        stereo_plugin_sidechain_port_groups
    };

    const plugin_metadata_t  sc_compressor_ms_metadata::metadata =
    {
        "Sidechain-Kompressor MidSide",
        "Sidechain Compressor MidSide",
        "SCK1MS",
        &developers::v_sadovnikov,
        "sc_compressor_ms",
        "ioqg",
        LSP_COMPRESSOR_BASE + 7,
        LSP_VERSION(1, 0, 1),
        compressor_classes,
        E_INLINE_DISPLAY,
        sc_compressor_ms_ports,
        "dynamics/compressor/single/ms.xml",
        NULL,
        stereo_plugin_sidechain_port_groups
    };
}
