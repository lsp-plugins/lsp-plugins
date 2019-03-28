/*
 * oscillator.cpp
 *
 *  Created on: 20 Mar 2017
 *      Author: crocoduck
 */

#include <metadata/plugins.h>
#include <metadata/ports.h>
#include <metadata/developers.h>

namespace lsp
{
    static const int oscillator_classes[] = { C_OSCILLATOR, -1};

    static const char *sc_func[] =
    {
        "Sine",
        "Cosine",
        "Squared Sine",
        "Squared Cosine",
        "Rectangular",
        "Sawtooth",
        "Trapezoid",
        "Pulsetrain",
        "Parabolic",
        "Band Limited Rectangular",
        "Band Limited Sawtooth",
        "Band Limited Trapezoid",
        "Band Limited Pulsetrain",
        "Band Limited Parabolic",
        NULL
    };

    static const char *sc_dc_ref[] =
    {
        "Wave DC",
        "Zero DC",
        NULL
    };

    static const char *sc_mode[] =
    {
        "Add",
        "Multiply",
        "Replace",
        NULL
    };

    static const char *sc_oversampler_mode[] =
    {
        "None",
        "x2",
        "x3",
        "x4",
        "x6",
        "x8",
        NULL
    };

    static const port_t oscillator_ports[] =
    {
        PORTS_MONO_PLUGIN,
        BYPASS,
        LOG_CONTROL("freq", "Frequency", U_HZ, oscillator_mono_metadata::FREQUENCY),
        AMP_GAIN10("gain", "Output gain", 1.0f),
        CONTROL("dcoff", "DC Offset", U_NONE, oscillator_mono_metadata::DCOFFSET),
        COMBO("scr", "DC Reference", oscillator_mono_metadata::SC_DC_DFL, sc_dc_ref),
        CONTROL("iniph", "Initial Phase", U_DEG, oscillator_mono_metadata::INITPHASE),
        COMBO("scm", "Operation Mode", oscillator_mono_metadata::SC_MODE_DFL, sc_mode),
        COMBO("scom", "Oversampler Mode", oscillator_mono_metadata::SC_OVS_DFL, sc_oversampler_mode),
        COMBO("scf", "Function", oscillator_mono_metadata::SC_FUNC_DFL, sc_func),
        SWITCH("invss", "Invert Squared Sinusoids", 0.0f),
        SWITCH("invps", "Invert Parabolic Signal", 0.0f),
        PERCENTS("rdtrt", "Duty Ratio", 50.0f, 0.025f),
        PERCENTS("swdth", "Width", 50.0f, 0.025f),
        PERCENTS("trsrt", "Raise Ratio", 25.0f, 0.025f),
        PERCENTS("tflrt", "Fall Ratio", 25.0f, 0.025f),
        PERCENTS("tpwrt", "Positive Width", 25.0f, 0.025f),
        PERCENTS("tnwrt", "Negative Width", 25.0f, 0.025f),
        PERCENTS("pwdth", "Width", 100.0f, 0.025f),
        MESH("ow", "Output wave", 2, oscillator_mono_metadata::HISTORY_MESH_SIZE),

        PORTS_END
    };

    const plugin_metadata_t oscillator_mono_metadata::metadata =
    {
        "Oszillator Mono",
        "Oscillator Mono",
        "O1M", // Oscillator x1 Mono
        &developers::s_tronci,
        "oscillator_mono",
        "sntq",
        LSP_OSCILLATOR_BASE + 0,
        LSP_VERSION(1, 0, 0),
        oscillator_classes,
        E_INLINE_DISPLAY,
        oscillator_ports,
        "util/oscillator_mono.xml",
        mono_plugin_port_groups
    };
}
