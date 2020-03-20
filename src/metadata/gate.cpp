/*
 * gate.cpp
 *
 *  Created on: 7 нояб. 2016 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <metadata/plugins.h>
#include <metadata/ports.h>
#include <metadata/developers.h>

namespace lsp
{
    //-------------------------------------------------------------------------
    // Gate
    static const int gate_classes[] = { C_GATE, -1 };

    static const port_item_t gate_sc_modes[] =
    {
        { "Peak",       "sidechain.peak"           },
        { "RMS",        "sidechain.rms"            },
        { "Low-Pass",   "sidechain.lowpass"        },
        { "Uniform",    "sidechain.uniform"        },
        { NULL, NULL }
    };

    static const port_item_t gate_sc_sources[] =
    {
        { "Middle",     "sidechain.middle" },
        { "Side",       "sidechain.side" },
        { "Left",       "sidechain.left" },
        { "Right",      "sidechain.right" },
        { NULL, NULL }
    };

    static const port_item_t gate_sc_type[] =
    {
        { "Internal",   "sidechain.internal" },
        { "External",   "sidechain.external" },
        { NULL, NULL }
    };

    #define GATE_COMMON     \
        BYPASS,             \
        IN_GAIN,            \
        OUT_GAIN,           \
        SWITCH("pause", "Pause graph analysis", 0.0f), \
        TRIGGER("clear", "Clear graph analysis")

    #define GATE_MS_COMMON  \
        GATE_COMMON,        \
        SWITCH("msl", "Mid/Side listen", 0.0f)

    #define GATE_MONO_CHANNEL \
        COMBO("scm", "Sidechain mode", gate_base_metadata::SC_MODE_DFL, gate_sc_modes), \
        CONTROL("sla", "Sidechain lookahead", U_MSEC, gate_base_metadata::LOOKAHEAD), \
        SWITCH("scl", "Sidechain listen", 0.0f), \
        LOG_CONTROL("scr", "Sidechain reactivity", U_MSEC, gate_base_metadata::REACTIVITY), \
        AMP_GAIN100("scp", "Sidechain preamp", GAIN_AMP_0_DB)

    #define GATE_SC_MONO_CHANNEL \
        COMBO("sci", "Sidechain input", gate_base_metadata::SC_TYPE_DFL, gate_sc_type), \
        GATE_MONO_CHANNEL

    #define GATE_STEREO_CHANNEL(id, label) \
        COMBO("scm" id, "Sidechain mode" label, gate_base_metadata::SC_MODE_DFL, gate_sc_modes), \
        CONTROL("sla" id, "Sidechain lookahead" label, U_MSEC, gate_base_metadata::LOOKAHEAD), \
        SWITCH("scl" id, "Sidechain listen" label, 0.0f), \
        COMBO("scs" id, "Sidechain source" label, gate_base_metadata::SC_SOURCE_DFL, gate_sc_sources), \
        LOG_CONTROL("scr" id, "Sidechain reactivity" label, U_MSEC, gate_base_metadata::REACTIVITY), \
        AMP_GAIN100("scp" id, "Sidechain preamp" label, GAIN_AMP_0_DB)

    #define GATE_SC_STEREO_CHANNEL(id, label) \
        COMBO("sci" id, "Sidechain input" label, gate_base_metadata::SC_TYPE_DFL, gate_sc_type), \
        GATE_STEREO_CHANNEL(id, label)

    #define GATE_CHANNEL(id, label) \
        SWITCH("gh" id, "Hysteresis" label, 0.0f), \
        LOG_CONTROL("gt" id, "Threshold" label, U_GAIN_AMP, gate_base_metadata::THRESHOLD), \
        LOG_CONTROL("gz" id, "Zone size" label, U_GAIN_AMP, gate_base_metadata::ZONE), \
        LOG_CONTROL("ht" id, "Hysteresis threshold" label, U_GAIN_AMP, gate_base_metadata::H_THRESHOLD), \
        LOG_CONTROL("hz" id, "Hysteresis zone size" label, U_GAIN_AMP, gate_base_metadata::ZONE), \
        LOG_CONTROL("at" id, "Attack" label, U_MSEC, gate_base_metadata::ATTACK_TIME), \
        LOG_CONTROL("rt" id, "Release" label, U_MSEC, gate_base_metadata::RELEASE_TIME), \
        LOG_CONTROL("gr" id, "Reduction" label, U_GAIN_AMP, gate_base_metadata::REDUCTION), \
        LOG_CONTROL("mk" id, "Makeup gain" label, U_GAIN_AMP, gate_base_metadata::MAKEUP), \
        AMP_GAIN10("cdr" id, "Dry gain" label, GAIN_AMP_M_INF_DB),     \
        AMP_GAIN10("cwt" id, "Wet gain" label, GAIN_AMP_0_DB), \
        SWITCH("slv" id, "Sidechain level visibility" label, 1.0f), \
        SWITCH("elv" id, "Envelope level visibility" label, 1.0f), \
        SWITCH("grv" id, "Gain reduction visibility" label, 1.0f), \
        METER_OUT_GAIN("gzs" id, "Zone start" label, GAIN_AMP_P_24_DB), \
        METER_OUT_GAIN("hts" id, "Hysteresis threshold start" label, GAIN_AMP_P_24_DB), \
        METER_OUT_GAIN("hzs" id, "Hysteresis zone start" label, GAIN_AMP_P_24_DB), \
        MESH("cg" id, "Curve graph" label, 2, gate_base_metadata::CURVE_MESH_SIZE), \
        MESH("hg" id, "Hysteresis graph" label, 2, gate_base_metadata::CURVE_MESH_SIZE), \
        MESH("scg" id, "Sidechain graph" label, 2, gate_base_metadata::TIME_MESH_SIZE), \
        MESH("evg" id, "Envelope graph" label, 2, gate_base_metadata::TIME_MESH_SIZE), \
        MESH("grg" id, "Gain reduciton graph" label, 2, gate_base_metadata::TIME_MESH_SIZE), \
        METER_OUT_GAIN("slm" id, "Sidechain level meter" label, GAIN_AMP_P_24_DB), \
        METER_OUT_GAIN("clm" id, "Curve level meter" label, GAIN_AMP_P_24_DB), \
        METER_OUT_GAIN("elm" id, "Envelope level meter" label, GAIN_AMP_P_24_DB), \
        METER_GAIN("rlm" id, "Reduction level meter" label, GAIN_AMP_0_DB)

    #define GATE_AUDIO_METER(id, label) \
        SWITCH("ilv" id, "Input level visibility" label, 1.0f), \
        SWITCH("olv" id, "Output level visibility" label, 1.0f), \
        MESH("icg" id, "Gate input" label, 2, gate_base_metadata::TIME_MESH_SIZE), \
        MESH("ocg" id, "Gate output" label, 2, gate_base_metadata::TIME_MESH_SIZE), \
        METER_GAIN("ilm" id, "Input level meter" label, GAIN_AMP_P_24_DB), \
        METER_GAIN("olm" id, "Output level meter" label, GAIN_AMP_P_24_DB)

    static const port_t gate_mono_ports[] =
    {
        PORTS_MONO_PLUGIN,
        GATE_COMMON,
        GATE_MONO_CHANNEL,
        GATE_CHANNEL("", ""),
        GATE_AUDIO_METER("", ""),

        PORTS_END
    };

    static const port_t gate_stereo_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        GATE_COMMON,
        GATE_STEREO_CHANNEL("", ""),
        GATE_CHANNEL("", ""),
        GATE_AUDIO_METER("_l", " Left"),
        GATE_AUDIO_METER("_r", " Right"),

        PORTS_END
    };

    static const port_t gate_lr_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        GATE_COMMON,
        GATE_STEREO_CHANNEL("_l", " Left"),
        GATE_STEREO_CHANNEL("_r", " Right"),
        GATE_CHANNEL("_l", " Left"),
        GATE_CHANNEL("_r", " Right"),
        GATE_AUDIO_METER("_l", " Left"),
        GATE_AUDIO_METER("_r", " Right"),

        PORTS_END
    };

    static const port_t gate_ms_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        GATE_MS_COMMON,
        GATE_STEREO_CHANNEL("_m", " Mid"),
        GATE_STEREO_CHANNEL("_s", " Side"),
        GATE_CHANNEL("_m", " Mid"),
        GATE_CHANNEL("_s", " Side"),
        GATE_AUDIO_METER("_m", " Mid"),
        GATE_AUDIO_METER("_s", " Side"),

        PORTS_END
    };

    static const port_t sc_gate_mono_ports[] =
    {
        PORTS_MONO_PLUGIN,
        PORTS_MONO_SIDECHAIN,
        GATE_COMMON,
        GATE_SC_MONO_CHANNEL,
        GATE_CHANNEL("", ""),
        GATE_AUDIO_METER("", ""),

        PORTS_END
    };

    static const port_t sc_gate_stereo_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        PORTS_STEREO_SIDECHAIN,
        GATE_COMMON,
        GATE_SC_STEREO_CHANNEL("", ""),
        GATE_CHANNEL("", ""),
        GATE_AUDIO_METER("_l", " Left"),
        GATE_AUDIO_METER("_r", " Right"),

        PORTS_END
    };

    static const port_t sc_gate_lr_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        PORTS_STEREO_SIDECHAIN,
        GATE_COMMON,
        GATE_SC_STEREO_CHANNEL("_l", " Left"),
        GATE_SC_STEREO_CHANNEL("_r", " Right"),
        GATE_CHANNEL("_l", " Left"),
        GATE_CHANNEL("_r", " Right"),
        GATE_AUDIO_METER("_l", " Left"),
        GATE_AUDIO_METER("_r", " Right"),

        PORTS_END
    };

    static const port_t sc_gate_ms_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        PORTS_STEREO_SIDECHAIN,
        GATE_MS_COMMON,
        GATE_SC_STEREO_CHANNEL("_m", " Mid"),
        GATE_SC_STEREO_CHANNEL("_s", " Side"),
        GATE_CHANNEL("_m", " Mid"),
        GATE_CHANNEL("_s", " Side"),
        GATE_AUDIO_METER("_m", " Mid"),
        GATE_AUDIO_METER("_s", " Side"),

        PORTS_END
    };

    // Gate
    const plugin_metadata_t  gate_mono_metadata::metadata =
    {
        "Gate Mono",
        "Gate Mono",
        "G1M",
        &developers::v_sadovnikov,
        "gate_mono",
        "ur0e",
        LSP_GATE_BASE + 0,
        LSP_VERSION(1, 0, 1),
        gate_classes,
        E_INLINE_DISPLAY,
        gate_mono_ports,
        "dynamics/gate/single/mono.xml",
        NULL,
        mono_plugin_port_groups
    };

    const plugin_metadata_t  gate_stereo_metadata::metadata =
    {
        "Gate Stereo",
        "Gate Stereo",
        "G1S",
        &developers::v_sadovnikov,
        "gate_stereo",
        "wg4o",
        LSP_GATE_BASE + 1,
        LSP_VERSION(1, 0, 1),
        gate_classes,
        E_INLINE_DISPLAY,
        gate_stereo_ports,
        "dynamics/gate/single/stereo.xml",
        NULL,
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  gate_lr_metadata::metadata =
    {
        "Gate LeftRight",
        "Gate LeftRight",
        "G1LR",
        &developers::v_sadovnikov,
        "gate_lr",
        "icmw",
        LSP_GATE_BASE + 2,
        LSP_VERSION(1, 0, 1),
        gate_classes,
        E_INLINE_DISPLAY,
        gate_lr_ports,
        "dynamics/gate/single/lr.xml",
        NULL,
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  gate_ms_metadata::metadata =
    {
        "Gate MidSide",
        "Gate MidSide",
        "G1MS",
        &developers::v_sadovnikov,
        "gate_ms",
        "zci1",
        LSP_GATE_BASE + 3,
        LSP_VERSION(1, 0, 1),
        gate_classes,
        E_INLINE_DISPLAY,
        gate_ms_ports,
        "dynamics/gate/single/ms.xml",
        NULL,
        stereo_plugin_port_groups
    };

    // Sidechain gate
    const plugin_metadata_t  sc_gate_mono_metadata::metadata =
    {
        "Sidechain-Gate Mono",
        "Sidechain Gate Mono",
        "SCG1M",
        &developers::v_sadovnikov,
        "sc_gate_mono",
        "nnz2",
        LSP_GATE_BASE + 4,
        LSP_VERSION(1, 0, 1),
        gate_classes,
        E_INLINE_DISPLAY,
        sc_gate_mono_ports,
        "dynamics/gate/single/mono.xml",
        NULL,
        mono_plugin_sidechain_port_groups
    };

    const plugin_metadata_t  sc_gate_stereo_metadata::metadata =
    {
        "Sidechain-Gate Stereo",
        "Sidechain Gate Stereo",
        "SCG1S",
        &developers::v_sadovnikov,
        "sc_gate_stereo",
        "fosg",
        LSP_GATE_BASE + 5,
        LSP_VERSION(1, 0, 1),
        gate_classes,
        E_INLINE_DISPLAY,
        sc_gate_stereo_ports,
        "dynamics/gate/single/stereo.xml",
        NULL,
        stereo_plugin_sidechain_port_groups
    };

    const plugin_metadata_t  sc_gate_lr_metadata::metadata =
    {
        "Sidechain-Gate LeftRight",
        "Sidechain Gate LeftRight",
        "SCG1LR",
        &developers::v_sadovnikov,
        "sc_gate_lr",
        "fmxo",
        LSP_GATE_BASE + 6,
        LSP_VERSION(1, 0, 1),
        gate_classes,
        E_INLINE_DISPLAY,
        sc_gate_lr_ports,
        "dynamics/gate/single/lr.xml",
        NULL,
        stereo_plugin_sidechain_port_groups
    };

    const plugin_metadata_t  sc_gate_ms_metadata::metadata =
    {
        "Sidechain-Gate MidSide",
        "Sidechain Gate MidSide",
        "SCG1MS",
        &developers::v_sadovnikov,
        "sc_gate_ms",
        "l6lc",
        LSP_GATE_BASE + 7,
        LSP_VERSION(1, 0, 1),
        gate_classes,
        E_INLINE_DISPLAY,
        sc_gate_ms_ports,
        "dynamics/gate/single/ms.xml",
        NULL,
        stereo_plugin_sidechain_port_groups
    };
}


