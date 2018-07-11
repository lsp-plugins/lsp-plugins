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

        // Bell
        "Bell RLC (BT)",
        "Bell RLC (MT)",
        "Bell BWC (BT)",
        "Bell BWC (MT)",
        "Bell LRX (BT)",
        "Bell LRX (MT)",

        // RLC Resonance filter
        "Resonance RLC (BT)",
        "Resonance RLC (MT)",

        // RLC Notch
        "Notch RLC (BT)",
        "Notch RLC (MT)",

        // Lo-pass
        "Lo-pass RLC (BT)",
        "Lo-pass RLC (MT)",
        "Lo-pass BWC (BT)",
        "Lo-pass BWC (MT)",
        "Lo-pass LRX (BT)",
        "Lo-pass LRX (MT)",

        // Lo-Shelf
        "Lo-shelf RLC (BT)",
        "Lo-shelf RLC (MT)",
        "Lo-shelf BWC (BT)",
        "Lo-shelf BWC (MT)",
        "Lo-shelf LRX (BT)",
        "Lo-shelf LRX (MT)",

        // Hi-pass
        "Hi-pass RLC (BT)",
        "Hi-pass RLC (MT)",
        "Hi-pass BWC (BT)",
        "Hi-pass BWC (MT)",
        "Hi-pass LRX (BT)",
        "Hi-pass LRX (MT)",

        // RLC Hi-shelf
        "Hi-shelf RLC (BT)",
        "Hi-shelf RLC (MT)",
        "Hi-shelf BWC (BT)",
        "Hi-shelf BWC (MT)",
        "Hi-shelf LRX (BT)",
        "Hi-shelf LRX (MT)",

        // Additional stuff
#ifndef LSP_NO_EXPERIMENTAL
        "Ladder-pass RLC (BT)",
        "Ladder-pass RLC (MT)",
        "Ladder-pass BWC (BT)",
        "Ladder-pass BWC (MT)",
        "Ladder-pass LRX (BT)",
        "Ladder-pass LRX (MT)",

        "Ladder-rej RLC (BT)",
        "Ladder-rej RLC (MT)",
        "Ladder-rej BWC (BT)",
        "Ladder-rej BWC (MT)",
        "Ladder-rej LRX (BT)",
        "Ladder-rej LRX (MT)",
#endif /* LSP_NO_EXPERIMENTAL */
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

    #define EQ_FILTER(id, label, x, total) \
            COMBO("ft" id "_" #x, "Filter type " label #x, 0, filter_types), \
            COMBO("s" id "_" #x, "Filter slope " label #x, 0, filter_slopes), \
            SWITCH("xs" id "_" #x, "Filter solo " label #x, 0.0f), \
            SWITCH("xm" id "_" #x, "Filter mute " label #x, 0.0f), \
            LOG_CONTROL("f" id "_" #x, "Frequency " label #x, U_HZ, para_equalizer_base_metadata::FREQ), \
            { "g" id "_" #x, "Gain " label # x, U_GAIN_AMP, R_CONTROL, F_IN | F_LOG | F_UPPER | F_LOWER | F_STEP, GAIN_AMP_M_36_DB, GAIN_AMP_P_36_DB, GAIN_AMP_0_DB, 0.1, NULL, NULL }, \
            { "q" id "_" #x, "Quality factor " label #x, U_NONE, R_CONTROL, F_IN | F_LOG | F_UPPER | F_LOWER | F_STEP, 0.0f, 100.0f, 0.0f, 0.01f, NULL    }, \
            { "hue" id "_" #x, "Hue " label #x, U_NONE, R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP, 0.0f, 1.0f, (float(x) / float(total)), 0.25f/360.0f, NULL     }, \
            BLINK("fv" id "_" #x, "Filter visibility " label #x), \
            MESH("agf" id "_" #x, "Amplitude graph " label #x, 2, para_equalizer_base_metadata::FILTER_MESH_POINTS)

    #define EQ_FILTER_MONO(x, total)    EQ_FILTER("", "", x, total)
    #define EQ_FILTER_STEREO(x, total)  EQ_FILTER("", "", x, total)
    #define EQ_FILTER_LR(x, total)      EQ_FILTER("l", "Left ", x, total), EQ_FILTER("r", "Right ", x, total)
    #define EQ_FILTER_MS(x, total)      EQ_FILTER("m", "Mid ", x, total), EQ_FILTER("s", "Side ", x, total)

    #define EQ_COMMON(fselect) \
            BYPASS, \
            AMP_GAIN("g_in", "Input gain", para_equalizer_base_metadata::IN_GAIN_DFL, 10.0f), \
            AMP_GAIN("g_out", "Output gain", para_equalizer_base_metadata::OUT_GAIN_DFL, 10.0f), \
            COMBO("mode", "Equalizer mode", 0, equalizer_eq_modes), \
            COMBO("fft", "FFT analysis", 0, equalizer_fft_mode), \
            LOG_CONTROL("react", "FFT reactivity", U_MSEC, para_equalizer_base_metadata::REACT_TIME), \
            AMP_GAIN("shift", "Shift gain", 1.0f, 100.0f), \
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
        EQ_FILTER_MONO(0, 16),
        EQ_FILTER_MONO(1, 16),
        EQ_FILTER_MONO(2, 16),
        EQ_FILTER_MONO(3, 16),
        EQ_FILTER_MONO(4, 16),
        EQ_FILTER_MONO(5, 16),
        EQ_FILTER_MONO(6, 16),
        EQ_FILTER_MONO(7, 16),
        EQ_FILTER_MONO(8, 16),
        EQ_FILTER_MONO(9, 16),
        EQ_FILTER_MONO(10, 16),
        EQ_FILTER_MONO(11, 16),
        EQ_FILTER_MONO(12, 16),
        EQ_FILTER_MONO(13, 16),
        EQ_FILTER_MONO(14, 16),
        EQ_FILTER_MONO(15, 16),

        PORTS_END
    };

    static const port_t para_equalizer_x32_mono_ports[] =
    {
        PORTS_MONO_PLUGIN,
        EQ_COMMON(filter_select_32),
        EQ_MONO_PORTS,
        EQ_FILTER_MONO(0, 32),
        EQ_FILTER_MONO(1, 32),
        EQ_FILTER_MONO(2, 32),
        EQ_FILTER_MONO(3, 32),
        EQ_FILTER_MONO(4, 32),
        EQ_FILTER_MONO(5, 32),
        EQ_FILTER_MONO(6, 32),
        EQ_FILTER_MONO(7, 32),
        EQ_FILTER_MONO(8, 32),
        EQ_FILTER_MONO(9, 32),
        EQ_FILTER_MONO(10, 32),
        EQ_FILTER_MONO(11, 32),
        EQ_FILTER_MONO(12, 32),
        EQ_FILTER_MONO(13, 32),
        EQ_FILTER_MONO(14, 32),
        EQ_FILTER_MONO(15, 32),
        EQ_FILTER_MONO(16, 32),
        EQ_FILTER_MONO(17, 32),
        EQ_FILTER_MONO(18, 32),
        EQ_FILTER_MONO(19, 32),
        EQ_FILTER_MONO(20, 32),
        EQ_FILTER_MONO(21, 32),
        EQ_FILTER_MONO(22, 32),
        EQ_FILTER_MONO(23, 32),
        EQ_FILTER_MONO(24, 32),
        EQ_FILTER_MONO(25, 32),
        EQ_FILTER_MONO(26, 32),
        EQ_FILTER_MONO(27, 32),
        EQ_FILTER_MONO(28, 32),
        EQ_FILTER_MONO(29, 32),
        EQ_FILTER_MONO(30, 32),
        EQ_FILTER_MONO(31, 32),

        PORTS_END
    };

    static const port_t para_equalizer_x16_stereo_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        EQ_COMMON(filter_select_16),
        EQ_STEREO_PORTS,
        EQ_FILTER_STEREO(0, 16),
        EQ_FILTER_STEREO(1, 16),
        EQ_FILTER_STEREO(2, 16),
        EQ_FILTER_STEREO(3, 16),
        EQ_FILTER_STEREO(4, 16),
        EQ_FILTER_STEREO(5, 16),
        EQ_FILTER_STEREO(6, 16),
        EQ_FILTER_STEREO(7, 16),
        EQ_FILTER_STEREO(8, 16),
        EQ_FILTER_STEREO(9, 16),
        EQ_FILTER_STEREO(10, 16),
        EQ_FILTER_STEREO(11, 16),
        EQ_FILTER_STEREO(12, 16),
        EQ_FILTER_STEREO(13, 16),
        EQ_FILTER_STEREO(14, 16),
        EQ_FILTER_STEREO(15, 16),

        PORTS_END
    };

    static const port_t para_equalizer_x32_stereo_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        EQ_COMMON(filter_select_32),
        EQ_STEREO_PORTS,
        EQ_FILTER_STEREO(0, 32),
        EQ_FILTER_STEREO(1, 32),
        EQ_FILTER_STEREO(2, 32),
        EQ_FILTER_STEREO(3, 32),
        EQ_FILTER_STEREO(4, 32),
        EQ_FILTER_STEREO(5, 32),
        EQ_FILTER_STEREO(6, 32),
        EQ_FILTER_STEREO(7, 32),
        EQ_FILTER_STEREO(8, 32),
        EQ_FILTER_STEREO(9, 32),
        EQ_FILTER_STEREO(10, 32),
        EQ_FILTER_STEREO(11, 32),
        EQ_FILTER_STEREO(12, 32),
        EQ_FILTER_STEREO(13, 32),
        EQ_FILTER_STEREO(14, 32),
        EQ_FILTER_STEREO(15, 32),
        EQ_FILTER_STEREO(16, 32),
        EQ_FILTER_STEREO(17, 32),
        EQ_FILTER_STEREO(18, 32),
        EQ_FILTER_STEREO(19, 32),
        EQ_FILTER_STEREO(20, 32),
        EQ_FILTER_STEREO(21, 32),
        EQ_FILTER_STEREO(22, 32),
        EQ_FILTER_STEREO(23, 32),
        EQ_FILTER_STEREO(24, 32),
        EQ_FILTER_STEREO(25, 32),
        EQ_FILTER_STEREO(26, 32),
        EQ_FILTER_STEREO(27, 32),
        EQ_FILTER_STEREO(28, 32),
        EQ_FILTER_STEREO(29, 32),
        EQ_FILTER_STEREO(30, 32),
        EQ_FILTER_STEREO(31, 32),

        PORTS_END
    };

    static const port_t para_equalizer_x16_lr_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        EQ_COMMON(filter_select_16lr),
        EQ_LR_PORTS,
        EQ_FILTER_LR(0, 16),
        EQ_FILTER_LR(1, 16),
        EQ_FILTER_LR(2, 16),
        EQ_FILTER_LR(3, 16),
        EQ_FILTER_LR(4, 16),
        EQ_FILTER_LR(5, 16),
        EQ_FILTER_LR(6, 16),
        EQ_FILTER_LR(7, 16),
        EQ_FILTER_LR(8, 16),
        EQ_FILTER_LR(9, 16),
        EQ_FILTER_LR(10, 16),
        EQ_FILTER_LR(11, 16),
        EQ_FILTER_LR(12, 16),
        EQ_FILTER_LR(13, 16),
        EQ_FILTER_LR(14, 16),
        EQ_FILTER_LR(15, 16),

        PORTS_END
    };

    static const port_t para_equalizer_x32_lr_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        EQ_COMMON(filter_select_32lr),
        EQ_LR_PORTS,
        EQ_FILTER_LR(0, 32),
        EQ_FILTER_LR(1, 32),
        EQ_FILTER_LR(2, 32),
        EQ_FILTER_LR(3, 32),
        EQ_FILTER_LR(4, 32),
        EQ_FILTER_LR(5, 32),
        EQ_FILTER_LR(6, 32),
        EQ_FILTER_LR(7, 32),
        EQ_FILTER_LR(8, 32),
        EQ_FILTER_LR(9, 32),
        EQ_FILTER_LR(10, 32),
        EQ_FILTER_LR(11, 32),
        EQ_FILTER_LR(12, 32),
        EQ_FILTER_LR(13, 32),
        EQ_FILTER_LR(14, 32),
        EQ_FILTER_LR(15, 32),
        EQ_FILTER_LR(16, 32),
        EQ_FILTER_LR(17, 32),
        EQ_FILTER_LR(18, 32),
        EQ_FILTER_LR(19, 32),
        EQ_FILTER_LR(20, 32),
        EQ_FILTER_LR(21, 32),
        EQ_FILTER_LR(22, 32),
        EQ_FILTER_LR(23, 32),
        EQ_FILTER_LR(24, 32),
        EQ_FILTER_LR(25, 32),
        EQ_FILTER_LR(26, 32),
        EQ_FILTER_LR(27, 32),
        EQ_FILTER_LR(28, 32),
        EQ_FILTER_LR(29, 32),
        EQ_FILTER_LR(30, 32),
        EQ_FILTER_LR(31, 32),

        PORTS_END
    };

    static const port_t para_equalizer_x16_ms_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        EQ_COMMON(filter_select_16ms),
        EQ_MS_PORTS,
        EQ_FILTER_MS(0, 16),
        EQ_FILTER_MS(1, 16),
        EQ_FILTER_MS(2, 16),
        EQ_FILTER_MS(3, 16),
        EQ_FILTER_MS(4, 16),
        EQ_FILTER_MS(5, 16),
        EQ_FILTER_MS(6, 16),
        EQ_FILTER_MS(7, 16),
        EQ_FILTER_MS(8, 16),
        EQ_FILTER_MS(9, 16),
        EQ_FILTER_MS(10, 16),
        EQ_FILTER_MS(11, 16),
        EQ_FILTER_MS(12, 16),
        EQ_FILTER_MS(13, 16),
        EQ_FILTER_MS(14, 16),
        EQ_FILTER_MS(15, 16),

        PORTS_END
    };

    static const port_t para_equalizer_x32_ms_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        EQ_COMMON(filter_select_32ms),
        EQ_MS_PORTS,
        EQ_FILTER_MS(0, 32),
        EQ_FILTER_MS(1, 32),
        EQ_FILTER_MS(2, 32),
        EQ_FILTER_MS(3, 32),
        EQ_FILTER_MS(4, 32),
        EQ_FILTER_MS(5, 32),
        EQ_FILTER_MS(6, 32),
        EQ_FILTER_MS(7, 32),
        EQ_FILTER_MS(8, 32),
        EQ_FILTER_MS(9, 32),
        EQ_FILTER_MS(10, 32),
        EQ_FILTER_MS(11, 32),
        EQ_FILTER_MS(12, 32),
        EQ_FILTER_MS(13, 32),
        EQ_FILTER_MS(14, 32),
        EQ_FILTER_MS(15, 32),
        EQ_FILTER_MS(16, 32),
        EQ_FILTER_MS(17, 32),
        EQ_FILTER_MS(18, 32),
        EQ_FILTER_MS(19, 32),
        EQ_FILTER_MS(20, 32),
        EQ_FILTER_MS(21, 32),
        EQ_FILTER_MS(22, 32),
        EQ_FILTER_MS(23, 32),
        EQ_FILTER_MS(24, 32),
        EQ_FILTER_MS(25, 32),
        EQ_FILTER_MS(26, 32),
        EQ_FILTER_MS(27, 32),
        EQ_FILTER_MS(28, 32),
        EQ_FILTER_MS(29, 32),
        EQ_FILTER_MS(30, 32),
        EQ_FILTER_MS(31, 32),

        PORTS_END
    };

    const plugin_metadata_t  para_equalizer_x16_mono_metadata::metadata =
    {
        "Parametrischer Entzerrer x16 Mono",
        "Parametric Equalizer",
        "PE16M",
        &developers::v_sadovnikov,
        "para_equalizer_x16_mono",
        "dh3y",
        LSP_PARA_EQUALIZER_BASE + 0,
        LSP_VERSION(1, 0, 1),
        para_equalizer_classes,
        para_equalizer_x16_mono_ports,
        NULL
    };

    const plugin_metadata_t  para_equalizer_x32_mono_metadata::metadata =
    {
        "Parametrischer Entzerrer x32 Mono",
        "Parametric Equalizer",
        "PE32M",
        &developers::v_sadovnikov,
        "para_equalizer_x32_mono",
        "i0px",
        LSP_PARA_EQUALIZER_BASE + 1,
        LSP_VERSION(1, 0, 1),
        para_equalizer_classes,
        para_equalizer_x32_mono_ports,
        NULL
    };

    const plugin_metadata_t  para_equalizer_x16_stereo_metadata::metadata =
    {
        "Parametrischer Entzerrer x16 Stereo",
        "Parametric Equalizer",
        "PE16S",
        &developers::v_sadovnikov,
        "para_equalizer_x16_stereo",
        "a5er",
        LSP_PARA_EQUALIZER_BASE + 2,
        LSP_VERSION(1, 0, 1),
        para_equalizer_classes,
        para_equalizer_x16_stereo_ports,
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  para_equalizer_x32_stereo_metadata::metadata =
    {
        "Parametrischer Entzerrer x32 Stereo",
        "Parametric Equalizer",
        "PE32S",
        &developers::v_sadovnikov,
        "para_equalizer_x32_stereo",
        "s2nz",
        LSP_PARA_EQUALIZER_BASE + 3,
        LSP_VERSION(1, 0, 1),
        para_equalizer_classes,
        para_equalizer_x32_stereo_ports,
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  para_equalizer_x16_lr_metadata::metadata =
    {
        "Parametrischer Entzerrer x16 LeftRight",
        "Parametric Equalizer",
        "PE16LR",
        &developers::v_sadovnikov,
        "para_equalizer_x16_lr",
        "4kef",
        LSP_PARA_EQUALIZER_BASE + 4,
        LSP_VERSION(1, 0, 1),
        para_equalizer_classes,
        para_equalizer_x16_lr_ports,
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  para_equalizer_x32_lr_metadata::metadata =
    {
        "Parametrischer Entzerrer x32 LeftRight",
        "Parametric Equalizer",
        "PE32LR",
        &developers::v_sadovnikov,
        "para_equalizer_x32_lr",
        "ilqj",
        LSP_PARA_EQUALIZER_BASE + 5,
        LSP_VERSION(1, 0, 1),
        para_equalizer_classes,
        para_equalizer_x32_lr_ports,
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  para_equalizer_x16_ms_metadata::metadata =
    {
        "Parametrischer Entzerrer x16 MidSide",
        "Parametric Equalizer",
        "PE16MS",
        &developers::v_sadovnikov,
        "para_equalizer_x16_ms",
        "opjs",
        LSP_PARA_EQUALIZER_BASE + 6,
        LSP_VERSION(1, 0, 1),
        para_equalizer_classes,
        para_equalizer_x16_ms_ports,
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  para_equalizer_x32_ms_metadata::metadata =
    {
        "Parametrischer Entzerrer x32 MidSide",
        "Parametric Equalizer",
        "PE32MS",
        &developers::v_sadovnikov,
        "para_equalizer_x32_ms",
        "lgz9",
        LSP_PARA_EQUALIZER_BASE + 7,
        LSP_VERSION(1, 0, 1),
        para_equalizer_classes,
        para_equalizer_x32_ms_ports,
        stereo_plugin_port_groups
    };
}



