/*
 * profiler.cpp
 *
 *  Created on: 12 Sep 2017
 *      Author: crocoduck
 */

#include <metadata/plugins.h>
#include <metadata/ports.h>
#include <metadata/developers.h>

namespace lsp
{
    static const int profiler_classes[] = {C_ANALYSER, C_UTILITY, -1};

    static const char *profiler_states[] =
    {
        "Idle",
        "Calibration",
        "Latency Detection",
        "Preprocessing",
        "Waiting",
        "Recording",
        "Convolving",
        "Postprocessing",
        "Saving",
        NULL
    };

    static const char *sc_rtalgo[] =
    {
        "EDT0",
        "EDT1",
        "RT10",
        "RT20",
        "RT30",
        NULL
    };

    static const char *sc_savemode[] =
    {
        "LTI Auto (*.wav)",
        "LTI RT (*.wav)",
        "LTI Coarse (*.wav)",
        "LTI All (*.wav)",
        "All Info (*.lspc)",
        NULL
    };

    static const port_t profiler_ports[] =
    {
        PORTS_MONO_PLUGIN,
        BYPASS,
        { "stld", "State LED", U_ENUM, R_METER, F_OUT | F_INT, 0, 0, 0, 0, profiler_states },
        LOG_CONTROL("calf", "Frequency", U_HZ, profiler_mono_metadata::FREQUENCY),
        AMP_GAIN10("cala", "Amplitude", profiler_mono_metadata::AMPLITUDE_DFL),
        SWITCH("cals", "Calibration", 0.0f),
        CONTROL("ltdm", "Max expected latency", U_MSEC, profiler_mono_metadata::LATENCY),
        CONTROL("ltdp", "Peak threshold", U_GAIN_AMP, profiler_mono_metadata::PEAK_THRESHOLD),
        CONTROL("ltda", "Absolute threshold", U_GAIN_AMP, profiler_mono_metadata::ABS_THRESHOLD),
        SWITCH("ltsk", "Skip Latency Detection", 0.0f),
        BLINK("skld", "Skip Latency Detection LED"),
        CONTROL("tsgl", "Duration", U_SEC, profiler_mono_metadata::DURATION),
        METER("tind", "Actual Signal Duration", U_SEC, profiler_mono_metadata::MTR_T),
        CONTROL("offc", "IR Time Offset", U_MSEC, profiler_mono_metadata::IR_OFFSET),
        METER("ltind", "Latency Value", U_MSEC, profiler_mono_metadata::MTR_LATENCY),
        COMBO("scra", "RT Algorithm", profiler_mono_metadata::SC_RTALGO_DFL, sc_rtalgo),
        METER("rtind", "Reverberation Time", U_SEC, profiler_mono_metadata::MTR_RT),
        BLINK("rta", "Reverberation Time Accuracy"),
        METER("ilind", "Integration Time", U_SEC, profiler_mono_metadata::MTR_IL),
        METER("rind", "Regression Line Correlation", U_NONE, profiler_mono_metadata::MTR_R),
        METER_GAIN20("ilvl", "Input Level"),
        TRIGGER("post", "Trig Post Processing"),
        COMBO("scsv", "Save Mode", profiler_mono_metadata::SC_SVMODE_DFL, sc_savemode),
        PATH("irfn", "Save file name"),
        TRIGGER("irfc", "Save file command"),
        STATUS("irfs", "File saving status"),
        METER_PERCENT("irfp", "File saving progress"),
        TRIGGER("latt", "Trig a Latency measurement"),
        TRIGGER("lint", "Trig a Linear measurement"),
        SWITCH("fbck", "Feedback", 0.0f),
        MESH("resdy", "Result", 2, profiler_mono_metadata::RESULT_MESH_SIZE),

        PORTS_END
    };

    const plugin_metadata_t profiler_mono_metadata::metadata =
    {
        "Profiler Mono",
        "Profiler Mono",
        "P1M", // Profiler x1 Mono
        &developers::s_tronci,
        "profiler_mono",
        "hwrc",
        0,
        LSP_VERSION(1, 0, 0),
        profiler_classes,
        profiler_ports,
        "util/profiler_mono.xml",
        mono_plugin_port_groups
    };
}
