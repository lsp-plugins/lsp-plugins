/*
 * spectrum_analyzer.cpp
 *
*  Created on: 08 апр. 2016 г.
 *      Author: sadko
 */

#include <metadata/plugins.h>
#include <metadata/ports.h>
#include <metadata/developers.h>

namespace lsp
{
    //-------------------------------------------------------------------------
    // Spectrum analyser: x1, x8, x12, x16, x24, x32
    static const int spectrum_analyzer_classes[] = { C_ANALYSER, C_UTILITY, C_SPECTRAL, -1 };

    static const char *fft_tolerance[] =
    {
        "1024",
        "2048",
        "4096",
        "8192",
        "16384",
        NULL
    };

    static const char *spectrum_analyzer_x1_channels[]=
    {
        "0",
        NULL
    };

    static const char *spectrum_analyzer_x1_modes[]=
    {
        "Analyzer",
        "Mastering",
        "Spectralizer",
        NULL
    };

    static const char *spectrum_analyzer_x2_channels[]=
    {
        "0",
        "1",
        NULL
    };

    static const char *spectrum_analyzer_x2_modes[]=
    {
        "Analyzer",
        "Mastering",
        "Spectralizer",
        "Spectralizer Stereo",
        NULL
    };

    static const char *spectrum_analyzer_x4_channels[]=
    {
        "0", "1", "2", "3",
        NULL
    };

    static const char *spectrum_analyzer_x4_modes[]=
    {
        "Analyzer",
        "Analyzer Stereo",
        "Mastering",
        "Mastering Stereo",
        "Spectralizer",
        "Spectralizer Stereo",
        NULL
    };

    static const char *spectrum_analyzer_x8_channels[]=
    {
        "0", "1", "2", "3", "4", "5", "6", "7",
        NULL
    };

    static const char **spectrum_analyzer_x8_modes = spectrum_analyzer_x4_modes;

    static const char *spectrum_analyzer_x12_channels[]=
    {
        "0", "1", "2", "3", "4", "5", "6", "7",
        "8", "9", "10", "11",
        NULL
    };

    static const char **spectrum_analyzer_x12_modes = spectrum_analyzer_x4_modes;

    static const char *spectrum_analyzer_x16_channels[]=
    {
        "0", "1", "2", "3", "4", "5", "6", "7",
        "8", "9", "10", "11", "12", "13", "14", "15",
        NULL
    };

    static const char *spectralizer_modes[] =
    {
        "Rainbow",
        "Fog",
        "Color",
        "Lightning",
        "Lightness",
        NULL
    };

    static const char **spectrum_analyzer_x16_modes = spectrum_analyzer_x4_modes;

    #define SA_INPUT(x, total) \
        AUDIO_INPUT_N(x), \
        AUDIO_OUTPUT_N(x), \
        { "on_" #x, "Analyse " #x, U_BOOL, R_CONTROL, F_IN, 0, 0, (x == 0) ? 1.0f : 0.0f, 0, NULL    }, \
        { "solo_" #x, "Solo " #x, U_BOOL, R_CONTROL, F_IN, 0, 0, 0, 0, NULL    }, \
        { "frz_" #x, "Freeze " #x, U_BOOL, R_CONTROL, F_IN, 0, 0, 0, 0, NULL    }, \
        { "hue_" #x, "Hue " #x, U_NONE, R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP, 0.0f, 1.0f, (float(x) / float(total)), 0.25f/360.0f, NULL     }, \
        AMP_GAIN("sh_" #x, "Shift gain " #x, 1.0f, 1000.0f), \
        MESH("spc_" #x, "Spectrum " #x, 2, spectrum_analyzer_base_metadata::MESH_POINTS)

    #define SA_COMMON(c) \
        BYPASS, \
        COMBO("mode", "Analyzer mode", 0, spectrum_analyzer_x ## c ## _modes), \
        COMBO("spm", "Spectralizer mode", 1, spectralizer_modes), \
        SWITCH("splog", "Spectralizer logarithmic scale", 1), \
        SWITCH("freeze", "Analyzer freeze", 0), \
        { "tol", "FFT Tolerance", U_ENUM, R_CONTROL, F_IN, 0, 0, spectrum_analyzer_base_metadata::RANK_DFL - spectrum_analyzer_base_metadata::RANK_MIN, 0, fft_tolerance }, \
        { "wnd", "FFT Window", U_ENUM, R_CONTROL, F_IN, 0, 0, spectrum_analyzer_base_metadata::WND_DFL, 0, windows::windows }, \
        { "env", "FFT Envelope", U_ENUM, R_CONTROL, F_IN, 0, 0, spectrum_analyzer_base_metadata::ENV_DFL, 0, envelope::envelopes }, \
        AMP_GAIN("pamp", "Preamp gain", spectrum_analyzer_base_metadata::PREAMP_DFL, 1000.0f), \
        LOG_CONTROL("zoom", "Graph zoom", U_GAIN_AMP, spectrum_analyzer_base_metadata::ZOOM), \
        { "react",          "Reactivity",       U_SEC,          R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP | F_LOG, \
             spectrum_analyzer_base_metadata::REACT_TIME_MIN, spectrum_analyzer_base_metadata::REACT_TIME_MAX, spectrum_analyzer_base_metadata::REACT_TIME_DFL, spectrum_analyzer_base_metadata::REACT_TIME_STEP, NULL }, \
        { "chn", "Channel", U_ENUM, R_CONTROL, F_IN, 0, 0, 0, 0, spectrum_analyzer_x ## c ## _channels }, \
        { "sel", "Selector", U_PERCENT, R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP | F_LOG, \
             spectrum_analyzer_base_metadata::SELECTOR_MIN, spectrum_analyzer_base_metadata::SELECTOR_MAX, spectrum_analyzer_base_metadata::SELECTOR_DFL, spectrum_analyzer_base_metadata::SELECTOR_STEP, NULL }, \
        { "freq", "Frequency", U_HZ, R_METER, F_OUT | F_UPPER | F_LOWER, \
            spectrum_analyzer_base_metadata::FREQ_MIN, spectrum_analyzer_base_metadata::FREQ_MAX, spectrum_analyzer_base_metadata::FREQ_DFL, 0, NULL }, \
        { "lvl", "Level", U_GAIN_AMP, R_METER, F_OUT | F_UPPER | F_LOWER, 0, 10000, 0, 0, NULL }

    static const port_t spectrum_analyzer_x1_ports[] =
    {
        SA_INPUT(0, 1),
        SA_COMMON(1),
        FBUFFER("fb", "Spectralizer buffer", spectrum_analyzer_base_metadata::FB_ROWS, spectrum_analyzer_base_metadata::MESH_POINTS),
        PORTS_END
    };

    static const port_t spectrum_analyzer_x2_ports[] =
    {
        SA_INPUT(0, 2),
        SA_INPUT(1, 2),
        SA_COMMON(2),
        COMBO("spc", "Spectralizer channel", 0, spectrum_analyzer_x2_channels),
        FBUFFER("fb0", "Spectralizer buffer 0", spectrum_analyzer_base_metadata::FB_ROWS, spectrum_analyzer_base_metadata::MESH_POINTS),
        FBUFFER("fb1", "Spectralizer buffer 1", spectrum_analyzer_base_metadata::FB_ROWS, spectrum_analyzer_base_metadata::MESH_POINTS),
        PORTS_END
    };

    static const port_t spectrum_analyzer_x4_ports[] =
    {
        SA_INPUT(0, 4),
        SA_INPUT(1, 4),
        SA_INPUT(2, 4),
        SA_INPUT(3, 4),
        SA_COMMON(4),
        COMBO("spc0", "Spectralizer channel 0", 0, spectrum_analyzer_x4_channels),
        FBUFFER("fb0", "Spectralizer buffer 0", spectrum_analyzer_base_metadata::FB_ROWS, spectrum_analyzer_base_metadata::MESH_POINTS),
        COMBO("spc1", "Spectralizer channel 1", 1, spectrum_analyzer_x4_channels),
        FBUFFER("fb1", "Spectralizer buffer 1", spectrum_analyzer_base_metadata::FB_ROWS, spectrum_analyzer_base_metadata::MESH_POINTS),
        PORTS_END
    };

    static const port_t spectrum_analyzer_x8_ports[] =
    {
        SA_INPUT(0, 8),
        SA_INPUT(1, 8),
        SA_INPUT(2, 8),
        SA_INPUT(3, 8),
        SA_INPUT(4, 8),
        SA_INPUT(5, 8),
        SA_INPUT(6, 8),
        SA_INPUT(7, 8),
        SA_COMMON(8),
        COMBO("spc0", "Spectralizer channel 0", 0, spectrum_analyzer_x8_channels),
        FBUFFER("fb0", "Spectralizer buffer 0", spectrum_analyzer_base_metadata::FB_ROWS, spectrum_analyzer_base_metadata::MESH_POINTS),
        COMBO("spc1", "Spectralizer channel 1", 1, spectrum_analyzer_x8_channels),
        FBUFFER("fb1", "Spectralizer buffer 1", spectrum_analyzer_base_metadata::FB_ROWS, spectrum_analyzer_base_metadata::MESH_POINTS),
        PORTS_END
    };

    static const port_t spectrum_analyzer_x12_ports[] =
    {
        SA_INPUT(0, 12),
        SA_INPUT(1, 12),
        SA_INPUT(2, 12),
        SA_INPUT(3, 12),
        SA_INPUT(4, 12),
        SA_INPUT(5, 12),
        SA_INPUT(6, 12),
        SA_INPUT(7, 12),
        SA_INPUT(8, 12),
        SA_INPUT(9, 12),
        SA_INPUT(10, 12),
        SA_INPUT(11, 12),
        SA_COMMON(12),
        COMBO("spc0", "Spectralizer channel 0", 0, spectrum_analyzer_x12_channels),
        FBUFFER("fb0", "Spectralizer buffer 0", spectrum_analyzer_base_metadata::FB_ROWS, spectrum_analyzer_base_metadata::MESH_POINTS),
        COMBO("spc1", "Spectralizer channel 1", 1, spectrum_analyzer_x12_channels),
        FBUFFER("fb1", "Spectralizer buffer 1", spectrum_analyzer_base_metadata::FB_ROWS, spectrum_analyzer_base_metadata::MESH_POINTS),
        PORTS_END
    };

    static const port_t spectrum_analyzer_x16_ports[] =
    {
        SA_INPUT(0, 16),
        SA_INPUT(1, 16),
        SA_INPUT(2, 16),
        SA_INPUT(3, 16),
        SA_INPUT(4, 16),
        SA_INPUT(5, 16),
        SA_INPUT(6, 16),
        SA_INPUT(7, 16),
        SA_INPUT(8, 16),
        SA_INPUT(9, 16),
        SA_INPUT(10, 16),
        SA_INPUT(11, 16),
        SA_INPUT(12, 16),
        SA_INPUT(13, 16),
        SA_INPUT(14, 16),
        SA_INPUT(15, 16),
        SA_COMMON(16),
        COMBO("spc0", "Spectralizer channel 0", 0, spectrum_analyzer_x16_channels),
        FBUFFER("fb0", "Spectralizer buffer 0", spectrum_analyzer_base_metadata::FB_ROWS, spectrum_analyzer_base_metadata::MESH_POINTS),
        COMBO("spc1", "Spectralizer channel 1", 1, spectrum_analyzer_x16_channels),
        FBUFFER("fb1", "Spectralizer buffer 1", spectrum_analyzer_base_metadata::FB_ROWS, spectrum_analyzer_base_metadata::MESH_POINTS),
        PORTS_END
    };

    #undef SA_INPUT
    #undef SA_COMMON

    const plugin_metadata_t  spectrum_analyzer_x1_metadata::metadata =
    {
        "Spektrumanalysator x1",
        "Spectrum Analyzer x1",
        "SA1",
        &developers::v_sadovnikov,
        "spectrum_analyzer_x1",
        "qtez",
        LSP_SPECTRUM_ANALYZER_BASE + 0,
        LSP_VERSION(1, 0, 2),
        spectrum_analyzer_classes,
        spectrum_analyzer_x1_ports,
        "analyzer/spectrum/x1.xml",
        NULL
    };

    const plugin_metadata_t  spectrum_analyzer_x2_metadata::metadata =
    {
        "Spektrumanalysator x2",
        "Spectrum Analyzer x2",
        "SA2",
        &developers::v_sadovnikov,
        "spectrum_analyzer_x2",
        "aw7r",
        LSP_SPECTRUM_ANALYZER_BASE + 1,
        LSP_VERSION(1, 0, 2),
        spectrum_analyzer_classes,
        spectrum_analyzer_x2_ports,
        "analyzer/spectrum/x2.xml",
        NULL
    };

    const plugin_metadata_t  spectrum_analyzer_x4_metadata::metadata =
    {
        "Spektrumanalysator x4",
        "Spectrum Analyzer x4",
        "SA4",
        &developers::v_sadovnikov,
        "spectrum_analyzer_x4",
        "xzgo",
        LSP_SPECTRUM_ANALYZER_BASE + 2,
        LSP_VERSION(1, 0, 2),
        spectrum_analyzer_classes,
        spectrum_analyzer_x4_ports,
        "analyzer/spectrum/x4.xml",
        NULL
    };

    const plugin_metadata_t  spectrum_analyzer_x8_metadata::metadata =
    {
        "Spektrumanalysator x8",
        "Spectrum Analyzer x8",
        "SA8",
        &developers::v_sadovnikov,
        "spectrum_analyzer_x8",
        "e5hb",
        LSP_SPECTRUM_ANALYZER_BASE + 3,
        LSP_VERSION(1, 0, 2),
        spectrum_analyzer_classes,
        spectrum_analyzer_x8_ports,
        "analyzer/spectrum/x8.xml",
        NULL
    };

    const plugin_metadata_t  spectrum_analyzer_x12_metadata::metadata =
    {
        "Spektrumanalysator x12",
        "Spectrum Analyzer x12",
        "SA12",
        &developers::v_sadovnikov,
        "spectrum_analyzer_x12",
        "tj3l",
        LSP_SPECTRUM_ANALYZER_BASE + 4,
        LSP_VERSION(1, 0, 2),
        spectrum_analyzer_classes,
        spectrum_analyzer_x12_ports,
        "analyzer/spectrum/x12.xml",
        NULL
    };

    const plugin_metadata_t  spectrum_analyzer_x16_metadata::metadata =
    {
        "Spektrumanalysator x16",
        "Spectrum Analyzer x16",
        "SA16",
        &developers::v_sadovnikov,
        "spectrum_analyzer_x16",
        "nuzi",
        LSP_SPECTRUM_ANALYZER_BASE + 5,
        LSP_VERSION(1, 0, 2),
        spectrum_analyzer_classes,
        spectrum_analyzer_x16_ports,
        "analyzer/spectrum/x16.xml",
        NULL
    };
}



