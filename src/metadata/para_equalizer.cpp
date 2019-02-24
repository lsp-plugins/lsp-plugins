/*
 * equalizer.cpp
 *
 *  Created on: 30 мая 2016 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <metadata/plugins.h>
#include <metadata/ports.h>
#include <metadata/developers.h>

namespace lsp
{
    //-------------------------------------------------------------------------
    // Parametric Equalizer
    static const int para_equalizer_classes[] = { C_FILTER, C_EQ, C_PARA_EQ, C_ANALYSER, -1 };

    static const char *filter_slopes[] =
    {
        "x1",
        "x2",
        "x3",
        "x4",
        NULL
    };

    static const char *equalizer_eq_modes[] =
    {
        "IIR",
        "FIR",
        "FFT",
        NULL
    };

    static const char *filter_types[] =
    {
        "Off",
        "Bell",
        "Hi-pass",
        "Hi-shelf",
        "Lo-pass",
        "Lo-shelf",
        "Notch",
        "Resonance",

        // Additional stuff
#ifndef LSP_NO_EXPERIMENTAL
        "Ladder-pass",
        "Ladder-rej",
        "Envelope",
        "Bandpass",
#endif /* LSP_NO_EXPERIMENTAL */
        NULL
    };

    static const char *filter_modes[] =
    {
        "RLC (BT)",
        "RLC (MT)",
        "BWC (BT)",
        "BWC (MT)",
        "LRX (BT)",
        "LRX (MT)",
        "APO",
        NULL
    };

    static const char *equalizer_fft_mode[] =
    {
        "Off",
        "Post-eq",
        "Pre-eq",
        NULL
    };

    static const char *filter_select_16[] =
    {
        "0-7",
        "8-15",
        NULL
    };

    static const char *filter_select_16lr[] =
    {
        "Left 0-7",
        "Right 0-7",
        "Left 8-15",
        "Right 8-15",
        NULL
    };

    static const char *filter_select_16ms[] =
    {
        "Middle 0-7",
        "Side 0-7",
        "Middle 8-15",
        "Side 8-15",
        NULL
    };

    static const char *filter_select_32[] =
    {
        "0-7",
        "8-15",
        "16-23",
        "24-31",
        NULL
    };

    static const char *filter_select_32lr[] =
    {
        "Left 0-7",
        "Right 0-7",
        "Left 8-15",
        "Right 8-15",
        "Left 16-23",
        "Right 16-23",
        "Left 24-31",
        "Right 24-31",
        NULL
    };

    static const char *filter_select_32ms[] =
    {
        "Mid 0-7",
        "Side 0-7",
        "Mid 8-15",
        "Side 8-15",
        "Mid 16-23",
        "Side 16-23",
        "Mid 24-31",
        "Side 24-31",
        NULL
    };

    #define EQ_FILTER(id, label, x, total, f) \
            COMBO("ft" id "_" #x, "Filter type " label #x, 0, filter_types), \
            COMBO("fm" id "_" #x, "Filter mode " label #x, 0, filter_modes), \
            COMBO("s" id "_" #x, "Filter slope " label #x, 0, filter_slopes), \
            SWITCH("xs" id "_" #x, "Filter solo " label #x, 0.0f), \
            SWITCH("xm" id "_" #x, "Filter mute " label #x, 0.0f), \
            LOG_CONTROL_DFL("f" id "_" #x, "Frequency " label #x, U_HZ, para_equalizer_base_metadata::FREQ, f), \
            { "g" id "_" #x, "Gain " label # x, U_GAIN_AMP, R_CONTROL, F_IN | F_LOG | F_UPPER | F_LOWER | F_STEP, GAIN_AMP_M_36_DB, GAIN_AMP_P_36_DB, GAIN_AMP_0_DB, 0.01, NULL, NULL }, \
            { "q" id "_" #x, "Quality factor " label #x, U_NONE, R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP, 0.0f, 100.0f, 0.0f, 0.025f, NULL        }, \
            { "hue" id "_" #x, "Hue " label #x, U_NONE, R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP, 0.0f, 1.0f, (float(x) / float(total)), 0.25f/360.0f, NULL     }, \
            BLINK("fv" id "_" #x, "Filter visibility " label #x), \
            MESH("agf" id "_" #x, "Amplitude graph " label #x, 2, para_equalizer_base_metadata::FILTER_MESH_POINTS)

    #define EQ_FILTER_MONO(x, total, f)     EQ_FILTER("", "", x, total, f)
    #define EQ_FILTER_STEREO(x, total, f)   EQ_FILTER("", "", x, total, f)
    #define EQ_FILTER_LR(x, total, f)       EQ_FILTER("l", "Left ", x, total, f), EQ_FILTER("r", "Right ", x, total, f)
    #define EQ_FILTER_MS(x, total, f)       EQ_FILTER("m", "Mid ", x, total, f), EQ_FILTER("s", "Side ", x, total, f)

    #define EQ_COMMON(fselect) \
            BYPASS, \
            AMP_GAIN("g_in", "Input gain", para_equalizer_base_metadata::IN_GAIN_DFL, 10.0f), \
            AMP_GAIN("g_out", "Output gain", para_equalizer_base_metadata::OUT_GAIN_DFL, 10.0f), \
            COMBO("mode", "Equalizer mode", 0, equalizer_eq_modes), \
            COMBO("fft", "FFT analysis", 0, equalizer_fft_mode), \
            LOG_CONTROL("react", "FFT reactivity", U_MSEC, para_equalizer_base_metadata::REACT_TIME), \
            AMP_GAIN("shift", "Shift gain", 1.0f, 100.0f), \
            LOG_CONTROL("zoom", "Graph zoom", U_GAIN_AMP, para_equalizer_base_metadata::ZOOM), \
            COMBO("fsel", "Filter select", 0, fselect)

    #define EQ_MONO_PORTS \
            MESH("ag", "Amplitude graph", 2, para_equalizer_base_metadata::MESH_POINTS), \
            METER_GAIN("sm", "Output signal meter", GAIN_AMP_P_12_DB), \
            MESH("fftg", "FFT graph", 2, para_equalizer_base_metadata::MESH_POINTS)

    #define EQ_STEREO_PORTS \
            PAN_CTL("bal", "Output balance", 0.0f), \
            MESH("ag", "Amplitude graph", 2, para_equalizer_base_metadata::MESH_POINTS), \
            METER_GAIN("sml", "Output signal meter Left", GAIN_AMP_P_12_DB), \
            MESH("fftg_l", "FFT channel Left", 2, para_equalizer_base_metadata::MESH_POINTS), \
            SWITCH("fftv_l", "FFT visibility Left", 1.0f), \
            METER_GAIN("smr", "Output signal meter Right", GAIN_AMP_P_12_DB), \
            MESH("fftg_r", "FFT channel Right", 2, para_equalizer_base_metadata::MESH_POINTS), \
            SWITCH("fftv_r", "FFT visibility Right", 1.0f)

    #define EQ_LR_PORTS \
            PAN_CTL("bal", "Output balance", 0.0f), \
            MESH("ag_l", "Amplitude graph Left", 2, para_equalizer_base_metadata::MESH_POINTS), \
            METER_GAIN("sml", "Output signal meter Left", GAIN_AMP_P_12_DB), \
            MESH("fftg_l", "FFT channel Left", 2, para_equalizer_base_metadata::MESH_POINTS), \
            SWITCH("fftv_l", "FFT visibility Left", 1.0f), \
            MESH("ag_r", "Amplitude graph Right", 2, para_equalizer_base_metadata::MESH_POINTS), \
            METER_GAIN("smr", "Output signal meter Right", GAIN_AMP_P_12_DB), \
            MESH("fftg_r", "FFT channel Right", 2, para_equalizer_base_metadata::MESH_POINTS), \
            SWITCH("fftv_r", "FFT visibility Right", 1.0f)

    #define EQ_MS_PORTS \
            PAN_CTL("bal", "Output balance", 0.0f), \
            SWITCH("lstn", "Mid/Side listen", 0.0f), \
            AMP_GAIN100("gain_m", "Mid gain", GAIN_AMP_0_DB), \
            AMP_GAIN100("gain_s", "Side gain", GAIN_AMP_0_DB), \
            MESH("ag_m", "Amplitude graph Mid", 2, para_equalizer_base_metadata::MESH_POINTS), \
            METER_GAIN("sml", "Output signal meter Left", GAIN_AMP_P_12_DB), \
            MESH("fftg_m", "FFT channel Mid", 2, para_equalizer_base_metadata::MESH_POINTS), \
            SWITCH("fftv_m", "FFT visibility Left", 1.0f), \
            MESH("ag_s", "Amplitude graph Side", 2, para_equalizer_base_metadata::MESH_POINTS), \
            METER_GAIN("smr", "Output signal meter Right", GAIN_AMP_P_12_DB), \
            MESH("fftg_s", "FFT channel Side", 2, para_equalizer_base_metadata::MESH_POINTS), \
            SWITCH("fftv_s", "FFT visibility Right", 1.0f)

    static const port_t para_equalizer_x16_mono_ports[] =
    {
        PORTS_MONO_PLUGIN,
        EQ_COMMON(filter_select_16),
        EQ_MONO_PORTS,
        EQ_FILTER_MONO(0, 16, 16.0f),
        EQ_FILTER_MONO(1, 16, 25.0f),
        EQ_FILTER_MONO(2, 16, 40.0f),
        EQ_FILTER_MONO(3, 16, 63.0f),
        EQ_FILTER_MONO(4, 16, 100.0f),
        EQ_FILTER_MONO(5, 16, 160.0f),
        EQ_FILTER_MONO(6, 16, 250.0f),
        EQ_FILTER_MONO(7, 16, 400.0f),
        EQ_FILTER_MONO(8, 16, 630.0f),
        EQ_FILTER_MONO(9, 16, 1000.0f),
        EQ_FILTER_MONO(10, 16, 1600.0f),
        EQ_FILTER_MONO(11, 16, 2500.0f),
        EQ_FILTER_MONO(12, 16, 4000.0f),
        EQ_FILTER_MONO(13, 16, 6300.0f),
        EQ_FILTER_MONO(14, 16, 10000.0f),
        EQ_FILTER_MONO(15, 16, 16000.0f),

        PORTS_END
    };

    static const port_t para_equalizer_x32_mono_ports[] =
    {
        PORTS_MONO_PLUGIN,
        EQ_COMMON(filter_select_32),
        EQ_MONO_PORTS,
        EQ_FILTER_MONO(0, 32, 16.0f),
        EQ_FILTER_MONO(1, 32, 20.0f),
        EQ_FILTER_MONO(2, 32, 25.0f),
        EQ_FILTER_MONO(3, 32, 31.5f),
        EQ_FILTER_MONO(4, 32, 40.0f),
        EQ_FILTER_MONO(5, 32, 50.0f),
        EQ_FILTER_MONO(6, 32, 63.0f),
        EQ_FILTER_MONO(7, 32, 80.0f),
        EQ_FILTER_MONO(8, 32, 100.0f),
        EQ_FILTER_MONO(9, 32, 125.0f),
        EQ_FILTER_MONO(10, 32, 160.0f),
        EQ_FILTER_MONO(11, 32, 200.0f),
        EQ_FILTER_MONO(12, 32, 250.0f),
        EQ_FILTER_MONO(13, 32, 315.0f),
        EQ_FILTER_MONO(14, 32, 400.0f),
        EQ_FILTER_MONO(15, 32, 500.0f),
        EQ_FILTER_MONO(16, 32, 630.0f),
        EQ_FILTER_MONO(17, 32, 800.0f),
        EQ_FILTER_MONO(18, 32, 1000.0f),
        EQ_FILTER_MONO(19, 32, 1250.0f),
        EQ_FILTER_MONO(20, 32, 1600.0f),
        EQ_FILTER_MONO(21, 32, 2000.0f),
        EQ_FILTER_MONO(22, 32, 2500.0f),
        EQ_FILTER_MONO(23, 32, 3150.0f),
        EQ_FILTER_MONO(24, 32, 4000.0f),
        EQ_FILTER_MONO(25, 32, 5000.0f),
        EQ_FILTER_MONO(26, 32, 6300.0f),
        EQ_FILTER_MONO(27, 32, 8000.0f),
        EQ_FILTER_MONO(28, 32, 10000.0f),
        EQ_FILTER_MONO(29, 32, 12500.0f),
        EQ_FILTER_MONO(30, 32, 16000.0f),
        EQ_FILTER_MONO(31, 32, 20000.0f),

        PORTS_END
    };

    static const port_t para_equalizer_x16_stereo_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        EQ_COMMON(filter_select_16),
        EQ_STEREO_PORTS,
        EQ_FILTER_STEREO(0, 16, 16.0f),
        EQ_FILTER_STEREO(1, 16, 25.0f),
        EQ_FILTER_STEREO(2, 16, 40.0f),
        EQ_FILTER_STEREO(3, 16, 63.0f),
        EQ_FILTER_STEREO(4, 16, 100.0f),
        EQ_FILTER_STEREO(5, 16, 160.0f),
        EQ_FILTER_STEREO(6, 16, 250.0f),
        EQ_FILTER_STEREO(7, 16, 400.0f),
        EQ_FILTER_STEREO(8, 16, 630.0f),
        EQ_FILTER_STEREO(9, 16, 1000.0f),
        EQ_FILTER_STEREO(10, 16, 1600.0f),
        EQ_FILTER_STEREO(11, 16, 2500.0f),
        EQ_FILTER_STEREO(12, 16, 4000.0f),
        EQ_FILTER_STEREO(13, 16, 6300.0f),
        EQ_FILTER_STEREO(14, 16, 10000.0f),
        EQ_FILTER_STEREO(15, 16, 16000.0f),

        PORTS_END
    };

    static const port_t para_equalizer_x32_stereo_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        EQ_COMMON(filter_select_32),
        EQ_STEREO_PORTS,
        EQ_FILTER_STEREO(0, 32, 16.0f),
        EQ_FILTER_STEREO(1, 32, 20.0f),
        EQ_FILTER_STEREO(2, 32, 25.0f),
        EQ_FILTER_STEREO(3, 32, 31.5f),
        EQ_FILTER_STEREO(4, 32, 40.0f),
        EQ_FILTER_STEREO(5, 32, 50.0f),
        EQ_FILTER_STEREO(6, 32, 63.0f),
        EQ_FILTER_STEREO(7, 32, 80.0f),
        EQ_FILTER_STEREO(8, 32, 100.0f),
        EQ_FILTER_STEREO(9, 32, 125.0f),
        EQ_FILTER_STEREO(10, 32, 160.0f),
        EQ_FILTER_STEREO(11, 32, 200.0f),
        EQ_FILTER_STEREO(12, 32, 250.0f),
        EQ_FILTER_STEREO(13, 32, 315.0f),
        EQ_FILTER_STEREO(14, 32, 400.0f),
        EQ_FILTER_STEREO(15, 32, 500.0f),
        EQ_FILTER_STEREO(16, 32, 630.0f),
        EQ_FILTER_STEREO(17, 32, 800.0f),
        EQ_FILTER_STEREO(18, 32, 1000.0f),
        EQ_FILTER_STEREO(19, 32, 1250.0f),
        EQ_FILTER_STEREO(20, 32, 1600.0f),
        EQ_FILTER_STEREO(21, 32, 2000.0f),
        EQ_FILTER_STEREO(22, 32, 2500.0f),
        EQ_FILTER_STEREO(23, 32, 3150.0f),
        EQ_FILTER_STEREO(24, 32, 4000.0f),
        EQ_FILTER_STEREO(25, 32, 5000.0f),
        EQ_FILTER_STEREO(26, 32, 6300.0f),
        EQ_FILTER_STEREO(27, 32, 8000.0f),
        EQ_FILTER_STEREO(28, 32, 10000.0f),
        EQ_FILTER_STEREO(29, 32, 12500.0f),
        EQ_FILTER_STEREO(30, 32, 16000.0f),
        EQ_FILTER_STEREO(31, 32, 20000.0f),

        PORTS_END
    };

    static const port_t para_equalizer_x16_lr_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        EQ_COMMON(filter_select_16lr),
        EQ_LR_PORTS,
        EQ_FILTER_LR(0, 16, 16.0f),
        EQ_FILTER_LR(1, 16, 25.0f),
        EQ_FILTER_LR(2, 16, 40.0f),
        EQ_FILTER_LR(3, 16, 63.0f),
        EQ_FILTER_LR(4, 16, 100.0f),
        EQ_FILTER_LR(5, 16, 160.0f),
        EQ_FILTER_LR(6, 16, 250.0f),
        EQ_FILTER_LR(7, 16, 400.0f),
        EQ_FILTER_LR(8, 16, 630.0f),
        EQ_FILTER_LR(9, 16, 1000.0f),
        EQ_FILTER_LR(10, 16, 1600.0f),
        EQ_FILTER_LR(11, 16, 2500.0f),
        EQ_FILTER_LR(12, 16, 4000.0f),
        EQ_FILTER_LR(13, 16, 6300.0f),
        EQ_FILTER_LR(14, 16, 10000.0f),
        EQ_FILTER_LR(15, 16, 16000.0f),

        PORTS_END
    };

    static const port_t para_equalizer_x32_lr_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        EQ_COMMON(filter_select_32lr),
        EQ_LR_PORTS,
        EQ_FILTER_LR(0, 32, 16.0f),
        EQ_FILTER_LR(1, 32, 20.0f),
        EQ_FILTER_LR(2, 32, 25.0f),
        EQ_FILTER_LR(3, 32, 31.5f),
        EQ_FILTER_LR(4, 32, 40.0f),
        EQ_FILTER_LR(5, 32, 50.0f),
        EQ_FILTER_LR(6, 32, 63.0f),
        EQ_FILTER_LR(7, 32, 80.0f),
        EQ_FILTER_LR(8, 32, 100.0f),
        EQ_FILTER_LR(9, 32, 125.0f),
        EQ_FILTER_LR(10, 32, 160.0f),
        EQ_FILTER_LR(11, 32, 200.0f),
        EQ_FILTER_LR(12, 32, 250.0f),
        EQ_FILTER_LR(13, 32, 315.0f),
        EQ_FILTER_LR(14, 32, 400.0f),
        EQ_FILTER_LR(15, 32, 500.0f),
        EQ_FILTER_LR(16, 32, 630.0f),
        EQ_FILTER_LR(17, 32, 800.0f),
        EQ_FILTER_LR(18, 32, 1000.0f),
        EQ_FILTER_LR(19, 32, 1250.0f),
        EQ_FILTER_LR(20, 32, 1600.0f),
        EQ_FILTER_LR(21, 32, 2000.0f),
        EQ_FILTER_LR(22, 32, 2500.0f),
        EQ_FILTER_LR(23, 32, 3150.0f),
        EQ_FILTER_LR(24, 32, 4000.0f),
        EQ_FILTER_LR(25, 32, 5000.0f),
        EQ_FILTER_LR(26, 32, 6300.0f),
        EQ_FILTER_LR(27, 32, 8000.0f),
        EQ_FILTER_LR(28, 32, 10000.0f),
        EQ_FILTER_LR(29, 32, 12500.0f),
        EQ_FILTER_LR(30, 32, 16000.0f),
        EQ_FILTER_LR(31, 32, 20000.0f),

        PORTS_END
    };

    static const port_t para_equalizer_x16_ms_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        EQ_COMMON(filter_select_16ms),
        EQ_MS_PORTS,
        EQ_FILTER_MS(0, 16, 16.0f),
        EQ_FILTER_MS(1, 16, 25.0f),
        EQ_FILTER_MS(2, 16, 40.0f),
        EQ_FILTER_MS(3, 16, 63.0f),
        EQ_FILTER_MS(4, 16, 100.0f),
        EQ_FILTER_MS(5, 16, 160.0f),
        EQ_FILTER_MS(6, 16, 250.0f),
        EQ_FILTER_MS(7, 16, 400.0f),
        EQ_FILTER_MS(8, 16, 630.0f),
        EQ_FILTER_MS(9, 16, 1000.0f),
        EQ_FILTER_MS(10, 16, 1600.0f),
        EQ_FILTER_MS(11, 16, 2500.0f),
        EQ_FILTER_MS(12, 16, 4000.0f),
        EQ_FILTER_MS(13, 16, 6300.0f),
        EQ_FILTER_MS(14, 16, 10000.0f),
        EQ_FILTER_MS(15, 16, 16000.0f),

        PORTS_END
    };

    static const port_t para_equalizer_x32_ms_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        EQ_COMMON(filter_select_32ms),
        EQ_MS_PORTS,
        EQ_FILTER_MS(0, 32, 16.0f),
        EQ_FILTER_MS(1, 32, 20.0f),
        EQ_FILTER_MS(2, 32, 25.0f),
        EQ_FILTER_MS(3, 32, 31.5f),
        EQ_FILTER_MS(4, 32, 40.0f),
        EQ_FILTER_MS(5, 32, 50.0f),
        EQ_FILTER_MS(6, 32, 63.0f),
        EQ_FILTER_MS(7, 32, 80.0f),
        EQ_FILTER_MS(8, 32, 100.0f),
        EQ_FILTER_MS(9, 32, 125.0f),
        EQ_FILTER_MS(10, 32, 160.0f),
        EQ_FILTER_MS(11, 32, 200.0f),
        EQ_FILTER_MS(12, 32, 250.0f),
        EQ_FILTER_MS(13, 32, 315.0f),
        EQ_FILTER_MS(14, 32, 400.0f),
        EQ_FILTER_MS(15, 32, 500.0f),
        EQ_FILTER_MS(16, 32, 630.0f),
        EQ_FILTER_MS(17, 32, 800.0f),
        EQ_FILTER_MS(18, 32, 1000.0f),
        EQ_FILTER_MS(19, 32, 1250.0f),
        EQ_FILTER_MS(20, 32, 1600.0f),
        EQ_FILTER_MS(21, 32, 2000.0f),
        EQ_FILTER_MS(22, 32, 2500.0f),
        EQ_FILTER_MS(23, 32, 3150.0f),
        EQ_FILTER_MS(24, 32, 4000.0f),
        EQ_FILTER_MS(25, 32, 5000.0f),
        EQ_FILTER_MS(26, 32, 6300.0f),
        EQ_FILTER_MS(27, 32, 8000.0f),
        EQ_FILTER_MS(28, 32, 10000.0f),
        EQ_FILTER_MS(29, 32, 12500.0f),
        EQ_FILTER_MS(30, 32, 16000.0f),
        EQ_FILTER_MS(31, 32, 20000.0f),

        PORTS_END
    };

    const plugin_metadata_t  para_equalizer_x16_mono_metadata::metadata =
    {
        "Parametrischer Entzerrer x16 Mono",
        "Parametric Equalizer x16 Mono",
        "PE16M",
        &developers::v_sadovnikov,
        "para_equalizer_x16_mono",
        "dh3y",
        LSP_PARA_EQUALIZER_BASE + 0,
        LSP_VERSION(1, 0, 3),
        para_equalizer_classes,
        para_equalizer_x16_mono_ports,
        "equalizer/parametric/x16/mono.xml",
        mono_plugin_port_groups
    };

    const plugin_metadata_t  para_equalizer_x32_mono_metadata::metadata =
    {
        "Parametrischer Entzerrer x32 Mono",
        "Parametric Equalizer x32 Mono",
        "PE32M",
        &developers::v_sadovnikov,
        "para_equalizer_x32_mono",
        "i0px",
        LSP_PARA_EQUALIZER_BASE + 1,
        LSP_VERSION(1, 0, 3),
        para_equalizer_classes,
        para_equalizer_x32_mono_ports,
        "equalizer/parametric/x32/mono.xml",
        mono_plugin_port_groups
    };

    const plugin_metadata_t  para_equalizer_x16_stereo_metadata::metadata =
    {
        "Parametrischer Entzerrer x16 Stereo",
        "Parametric Equalizer x16 Stereo",
        "PE16S",
        &developers::v_sadovnikov,
        "para_equalizer_x16_stereo",
        "a5er",
        LSP_PARA_EQUALIZER_BASE + 2,
        LSP_VERSION(1, 0, 3),
        para_equalizer_classes,
        para_equalizer_x16_stereo_ports,
        "equalizer/parametric/x16/stereo.xml",
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  para_equalizer_x32_stereo_metadata::metadata =
    {
        "Parametrischer Entzerrer x32 Stereo",
        "Parametric Equalizer x32 Stereo",
        "PE32S",
        &developers::v_sadovnikov,
        "para_equalizer_x32_stereo",
        "s2nz",
        LSP_PARA_EQUALIZER_BASE + 3,
        LSP_VERSION(1, 0, 3),
        para_equalizer_classes,
        para_equalizer_x32_stereo_ports,
        "equalizer/parametric/x32/stereo.xml",
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  para_equalizer_x16_lr_metadata::metadata =
    {
        "Parametrischer Entzerrer x16 LeftRight",
        "Parametric Equalizer x16 LeftRight",
        "PE16LR",
        &developers::v_sadovnikov,
        "para_equalizer_x16_lr",
        "4kef",
        LSP_PARA_EQUALIZER_BASE + 4,
        LSP_VERSION(1, 0, 3),
        para_equalizer_classes,
        para_equalizer_x16_lr_ports,
        "equalizer/parametric/x16/lr.xml",
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  para_equalizer_x32_lr_metadata::metadata =
    {
        "Parametrischer Entzerrer x32 LeftRight",
        "Parametric Equalizer x32 LeftRight",
        "PE32LR",
        &developers::v_sadovnikov,
        "para_equalizer_x32_lr",
        "ilqj",
        LSP_PARA_EQUALIZER_BASE + 5,
        LSP_VERSION(1, 0, 3),
        para_equalizer_classes,
        para_equalizer_x32_lr_ports,
        "equalizer/parametric/x32/lr.xml",
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  para_equalizer_x16_ms_metadata::metadata =
    {
        "Parametrischer Entzerrer x16 MidSide",
        "Parametric Equalizer x16 MidSide",
        "PE16MS",
        &developers::v_sadovnikov,
        "para_equalizer_x16_ms",
        "opjs",
        LSP_PARA_EQUALIZER_BASE + 6,
        LSP_VERSION(1, 0, 3),
        para_equalizer_classes,
        para_equalizer_x16_ms_ports,
        "equalizer/parametric/x16/ms.xml",
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  para_equalizer_x32_ms_metadata::metadata =
    {
        "Parametrischer Entzerrer x32 MidSide",
        "Parametric Equalizer x32 MidSide",
        "PE32MS",
        &developers::v_sadovnikov,
        "para_equalizer_x32_ms",
        "lgz9",
        LSP_PARA_EQUALIZER_BASE + 7,
        LSP_VERSION(1, 0, 3),
        para_equalizer_classes,
        para_equalizer_x32_ms_ports,
        "equalizer/parametric/x32/ms.xml",
        stereo_plugin_port_groups
    };
}



