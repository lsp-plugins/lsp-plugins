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

	#define CALIBRATOR \
		LOG_CONTROL("calf", "Frequency", U_HZ, profiler_mono_metadata::FREQUENCY), \
		AMP_GAIN10("cala", "Amplitude", profiler_mono_metadata::AMPLITUDE_DFL), \
		SWITCH("cals", "Calibration", 0.0f), \
		SWITCH("fbck", "Feedback", 0.0f)

	#define LATENCY_DETECTOR \
        CONTROL("ltdm", "Max expected latency", U_MSEC, profiler_mono_metadata::LATENCY), \
        CONTROL("ltdp", "Peak threshold", U_GAIN_AMP, profiler_mono_metadata::PEAK_THRESHOLD), \
        CONTROL("ltda", "Absolute threshold", U_GAIN_AMP, profiler_mono_metadata::ABS_THRESHOLD), \
        SWITCH("ltena", "Enable Latency Detection", 1.0f), \
		TRIGGER("latt", "Trig a Latency measurement")

	#define TEST_SIGNAL \
        CONTROL("tsgl", "Duration", U_SEC, profiler_mono_metadata::DURATION), \
		METER("tind", "Actual Signal Duration", U_SEC, profiler_mono_metadata::MTR_T), \
		TRIGGER("lint", "Trig a Linear measurement")

	#define POSTPROCESSOR \
		CONTROL("offc", "IR Time Offset", U_MSEC, profiler_mono_metadata::IR_OFFSET), \
		COMBO("scra", "RT Algorithm", profiler_mono_metadata::SC_RTALGO_DFL, sc_rtalgo), \
		TRIGGER("post", "Trig Post Processing")

	#define SAVER \
        COMBO("scsv", "Save Mode", profiler_mono_metadata::SC_SVMODE_DFL, sc_savemode), \
        PATH("irfn", "Save file name"), \
        TRIGGER("irfc", "Save file command"), \
        STATUS("irfs", "File saving status"), \
        METER_PERCENT("irfp", "File saving progress")

	#define PROFILER_COMMON \
		BYPASS, \
		{ "stld", "State LED", U_ENUM, R_METER, F_OUT | F_INT, 0, 0, 0, 0, profiler_states }, \
		CALIBRATOR, \
		LATENCY_DETECTOR, \
		TEST_SIGNAL, \
		POSTPROCESSOR, \
		SAVER

	#define PROFILER_VISUALOUTS(id, label) \
		METER_GAIN20("ilv" id, "Input Level" label), \
    	METER("lti" id, "Latency Value" label, U_MSEC, profiler_mono_metadata::MTR_LATENCY), \
		METER("rti" id, "Reverberation Time" label, U_SEC, profiler_mono_metadata::MTR_RT), \
		BLINK("rta" id, "Reverberation Time Accuracy" label), \
		METER("ili" id, "Integration Time" label, U_SEC, profiler_mono_metadata::MTR_IL), \
		METER("rci" id, "Regression Line Correlation" label, U_NONE, profiler_mono_metadata::MTR_R), \
		MESH("rme" id, "Result" label, 2, profiler_mono_metadata::RESULT_MESH_SIZE)

	#define PROFILER_VISUALOUTS_MONO 	PROFILER_VISUALOUTS("", "")
	#define PROFILER_VISUALOUTS_STEREO 	PROFILER_VISUALOUTS("_l", " Left"), PROFILER_VISUALOUTS("_r", " Right")

    static const port_t profiler_mono_ports[] =
    {
        PORTS_MONO_PLUGIN,
		PROFILER_COMMON,
		PROFILER_VISUALOUTS_MONO,
        PORTS_END
    };

    static const port_t profiler_stereo_ports[] =
    {
		PORTS_STEREO_PLUGIN,
		PROFILER_COMMON,
		PROFILER_VISUALOUTS_STEREO,
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
        profiler_mono_ports,
        "util/profiler/profiler_mono.xml",
        mono_plugin_port_groups
    };

    const plugin_metadata_t profiler_stereo_metadata::metadata =
    {
        "Profiler Stereo",
        "Profiler Stereo",
        "P1S", // Profiler x1 Stereo
        &developers::s_tronci,
        "profiler_stereo",
        "hubw",
        0,
        LSP_VERSION(1, 0, 0),
        profiler_classes,
        profiler_stereo_ports,
        "util/profiler/profiler_stereo.xml",
        stereo_plugin_port_groups
    };
}
