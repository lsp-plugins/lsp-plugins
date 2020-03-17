/*
 * equalizer.cpp
 *
 *  Created on: 11 авг. 2016 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <metadata/plugins.h>
#include <metadata/ports.h>
#include <metadata/developers.h>

namespace lsp
{
    //-------------------------------------------------------------------------
    // Graphic Equalizer
    static const int graph_equalizer_classes[] = { C_MULTI_EQ, -1 };

    static const port_item_t band_fft_mode[] =
    {
        { "Off",        "metering.fft.off" },
        { "Post-eq",    "metering.fft.post_eq" },
        { "Pre-eq",     "metering.fft.pre_eq" },
        { NULL, NULL }
    };

    static const port_item_t band_slopes[] =
    {
        { "BT48",       "graph_eq.slope.bt48" },
        { "MT48",       "graph_eq.slope.mt48" },
        { "BT72",       "graph_eq.slope.bt72" },
        { "MT72",       "graph_eq.slope.mt72" },
        { "BT96",       "graph_eq.slope.bt96" },
        { "MT96",       "graph_eq.slope.mt96" },
        { NULL, NULL }
    };

    static const port_item_t band_eq_modes[] =
    {
        { "IIR",        "eq.type.iir" },
        { "FIR",        "eq.type.fir" },
        { "FFT",        "eq.type.fft" },
        { NULL, NULL }
    };

    static const port_item_t band_select_16lr[] =
    {
        { "Bands Left",             "graph_eq.bands_l" },
        { "Bands Right",            "graph_eq.bands_r" },
        { NULL, NULL }
    };

    static const port_item_t band_select_16ms[] =
    {
        { "Bands Middle",           "graph_eq.bands_m" },
        { "Bands Side",             "graph_eq.bands_s" },
        { NULL, NULL }
    };

    static const port_item_t band_select_32[] =
    {
        { "Bands 0-15",             "graph_eq.bands_0:15" },
        { "Bands 16-31",            "graph_eq.bands_16:31" },
        { NULL, NULL }
    };

    static const port_item_t band_select_32lr[] =
    {
        { "Bands Left 0-15",        "graph_eq.bands_l_0:15" },
        { "Bands Right 0-15",       "graph_eq.bands_r_0:15" },
        { "Bands Left 16-31",       "graph_eq.bands_l_16:31" },
        { "Bands Right 16-31",      "graph_eq.bands_r_16:31" },
        { NULL, NULL }
    };

    static const port_item_t band_select_32ms[] =
    {
        { "Bands Middle 0-15",      "graph_eq.bands_m_0:15" },
        { "Bands Side 0-15",        "graph_eq.bands_s_0:15" },
        { "Bands Middle 16-31",     "graph_eq.bands_m_16:31" },
        { "Bands Side 16-31",       "graph_eq.bands_s_16:31" },
        { NULL, NULL }
    };

    #define EQ_BAND(id, label, x, f) \
        SWITCH("xs" id "_" #x, "Band solo" label " " f, 0.0f), \
        SWITCH("xm" id "_" #x, "Band mute" label " " f, 0.0f), \
        SWITCH("xe" id "_" #x, "Band on" label " " f, 1.0f), \
        BLINK("fv" id "_" #x, "Filter visibility " label " " f), \
        LOG_CONTROL("g" id "_" #x, "Band gain" label " " f, U_GAIN_AMP, graph_equalizer_base_metadata::BAND_GAIN)

    #define EQ_BAND_MONO(x, f)      EQ_BAND("", "", x, f)
    #define EQ_BAND_STEREO(x, f)    EQ_BAND("", "", x, f)
    #define EQ_BAND_LR(x, f)        EQ_BAND("l", " Left", x, f), EQ_BAND("r", " Right", x, f)
    #define EQ_BAND_MS(x, f)        EQ_BAND("m", " Mid", x, f), EQ_BAND("s", " Side", x, f)

    #define EQ_MONO_PORTS \
            MESH("ag", "Amplitude graph", 2, graph_equalizer_base_metadata::FILTER_MESH_POINTS), \
            METER_GAIN("im", "Input signal meter", GAIN_AMP_P_12_DB), \
            METER_GAIN("sm", "Output signal meter", GAIN_AMP_P_12_DB), \
            MESH("fftg", "FFT graph", 2, graph_equalizer_base_metadata::MESH_POINTS)

    #define EQ_STEREO_PORTS \
            PAN_CTL("bal", "Output balance", 0.0f), \
            MESH("ag", "Amplitude graph", 2, graph_equalizer_base_metadata::FILTER_MESH_POINTS), \
            METER_GAIN("iml", "Input signal meter Left", GAIN_AMP_P_12_DB), \
            METER_GAIN("sml", "Output signal meter Left", GAIN_AMP_P_12_DB), \
            MESH("fftg_l", "FFT channel Left", 2, graph_equalizer_base_metadata::MESH_POINTS), \
            SWITCH("fftv_l", "FFT visibility Left", 1.0f), \
            METER_GAIN("imr", "Input signal meter Right", GAIN_AMP_P_12_DB), \
            METER_GAIN("smr", "Output signal meter Right", GAIN_AMP_P_12_DB), \
            MESH("fftg_r", "FFT channel Right", 2, graph_equalizer_base_metadata::MESH_POINTS), \
            SWITCH("fftv_r", "FFT visibility Right", 1.0f)

    #define EQ_LR_PORTS \
            PAN_CTL("bal", "Output balance", 0.0f), \
            MESH("ag_l", "Amplitude graph Left", 2, graph_equalizer_base_metadata::FILTER_MESH_POINTS), \
            METER_GAIN("iml", "Input signal meter Left", GAIN_AMP_P_12_DB), \
            METER_GAIN("sml", "Output signal meter Left", GAIN_AMP_P_12_DB), \
            MESH("fftg_l", "FFT channel Left", 2, graph_equalizer_base_metadata::MESH_POINTS), \
            SWITCH("fftv_l", "FFT visibility Left", 1.0f), \
            MESH("ag_r", "Amplitude graph Right", 2, graph_equalizer_base_metadata::FILTER_MESH_POINTS), \
            METER_GAIN("imr", "Input signal meter Right", GAIN_AMP_P_12_DB), \
            METER_GAIN("smr", "Output signal meter Right", GAIN_AMP_P_12_DB), \
            MESH("fftg_r", "FFT channel Right", 2, graph_equalizer_base_metadata::MESH_POINTS), \
            SWITCH("fftv_r", "FFT visibility Right", 1.0f)

    #define EQ_MS_PORTS \
            PAN_CTL("bal", "Output balance", 0.0f), \
            SWITCH("lstn", "Mid/Side listen", 0.0f), \
            AMP_GAIN100("gain_m", "Mid gain", GAIN_AMP_0_DB), \
            AMP_GAIN100("gain_s", "Side gain", GAIN_AMP_0_DB), \
            MESH("ag_m", "Amplitude graph Mid", 2, graph_equalizer_base_metadata::FILTER_MESH_POINTS), \
            METER_GAIN("iml", "Input signal meter Left", GAIN_AMP_P_12_DB), \
            METER_GAIN("sml", "Output signal meter Left", GAIN_AMP_P_12_DB), \
            MESH("fftg_m", "FFT channel Mid", 2, graph_equalizer_base_metadata::MESH_POINTS), \
            SWITCH("fftv_m", "FFT visibility Left", 1.0f), \
            MESH("ag_s", "Amplitude graph Side", 2, graph_equalizer_base_metadata::FILTER_MESH_POINTS), \
            METER_GAIN("imr", "Input signal meter Right", GAIN_AMP_P_12_DB), \
            METER_GAIN("smr", "Output signal meter Right", GAIN_AMP_P_12_DB), \
            MESH("fftg_s", "FFT channel Side", 2, graph_equalizer_base_metadata::MESH_POINTS), \
            SWITCH("fftv_s", "FFT visibility Right", 1.0f)

    #define EQ_COMMON \
        BYPASS, \
        AMP_GAIN("g_in", "Input gain", graph_equalizer_base_metadata::IN_GAIN_DFL, 10.0f), \
        AMP_GAIN("g_out", "Output gain", graph_equalizer_base_metadata::OUT_GAIN_DFL, 10.0f), \
        COMBO("mode", "Equalizer mode", 0, band_eq_modes), \
        COMBO("slope", "Filter slope", 0, band_slopes), \
        COMBO("fft", "FFT analysis", 0, band_fft_mode), \
        LOG_CONTROL("react", "FFT reactivity", U_MSEC, graph_equalizer_base_metadata::REACT_TIME), \
        AMP_GAIN("shift", "Shift gain", 1.0f, 100.0f), \
        LOG_CONTROL("zoom", "Graph zoom", U_GAIN_AMP, graph_equalizer_base_metadata::ZOOM)

    #define BAND_SELECT(fselect) \
        COMBO("fsel", "Band select", 0, fselect)

    #define EQ_BANDS_32X(band) \
        band(0, "16"), \
        band(1, "20"), \
        band(2, "25"), \
        band(3, "31.5"), \
        band(4, "40"), \
        band(5, "50"), \
        band(6, "63"), \
        band(7, "80"), \
        band(8, "100"), \
        band(9, "125"), \
        band(10, "160"), \
        band(11, "200"), \
        band(12, "250"), \
        band(13, "315"), \
        band(14, "400"), \
        band(15, "500"), \
        band(16, "630"), \
        band(17, "800"), \
        band(18, "1K"), \
        band(19, "1.25K"), \
        band(20, "1.6K"), \
        band(21, "2K"), \
        band(22, "2.5K"), \
        band(23, "3.15K"), \
        band(24, "4K"), \
        band(25, "5K"), \
        band(26, "6.3K"), \
        band(27, "8K"), \
        band(28, "10K"), \
        band(29, "12.5K"), \
        band(30, "16K"), \
        band(31, "20K")

    #define EQ_BANDS_16X(band) \
        band(0, "16"), \
        band(1, "25"), \
        band(2, "40"), \
        band(3, "63"), \
        band(4, "100"), \
        band(5, "160"), \
        band(6, "250"), \
        band(7, "400"), \
        band(8, "630"), \
        band(9, "1K"), \
        band(10, "1.6K"), \
        band(11, "2.5K"), \
        band(12, "4K"), \
        band(13, "6.3K"), \
        band(14, "10K"), \
        band(15, "16K")

    static const port_t graph_equalizer_x16_mono_ports[] =
    {
        PORTS_MONO_PLUGIN,
        EQ_COMMON,
        EQ_MONO_PORTS,
        EQ_BANDS_16X(EQ_BAND_MONO),

        PORTS_END
    };

    static const port_t graph_equalizer_x16_stereo_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        EQ_COMMON,
        EQ_STEREO_PORTS,
        EQ_BANDS_16X(EQ_BAND_STEREO),

        PORTS_END
    };

    static const port_t graph_equalizer_x16_lr_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        EQ_COMMON,
        BAND_SELECT(band_select_16lr),
        EQ_LR_PORTS,
        EQ_BANDS_16X(EQ_BAND_LR),

        PORTS_END
    };

    static const port_t graph_equalizer_x16_ms_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        EQ_COMMON,
        BAND_SELECT(band_select_16ms),
        EQ_MS_PORTS,
        EQ_BANDS_16X(EQ_BAND_MS),

        PORTS_END
    };

    static const port_t graph_equalizer_x32_mono_ports[] =
    {
        PORTS_MONO_PLUGIN,
        EQ_COMMON,
        BAND_SELECT(band_select_32),
        EQ_MONO_PORTS,
        EQ_BANDS_32X(EQ_BAND_MONO),

        PORTS_END
    };

    static const port_t graph_equalizer_x32_stereo_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        EQ_COMMON,
        BAND_SELECT(band_select_32),
        EQ_STEREO_PORTS,
        EQ_BANDS_32X(EQ_BAND_STEREO),

        PORTS_END
    };

    static const port_t graph_equalizer_x32_lr_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        EQ_COMMON,
        BAND_SELECT(band_select_32lr),
        EQ_LR_PORTS,
        EQ_BANDS_32X(EQ_BAND_LR),

        PORTS_END
    };

    static const port_t graph_equalizer_x32_ms_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        EQ_COMMON,
        BAND_SELECT(band_select_32ms),
        EQ_MS_PORTS,
        EQ_BANDS_32X(EQ_BAND_MS),

        PORTS_END
    };

    const plugin_metadata_t  graph_equalizer_x16_mono_metadata::metadata =
    {
        "Grafischer Entzerrer x16 Mono",
        "Graphic Equalizer x16 Mono",
        "GE16M",
        &developers::v_sadovnikov,
        "graph_equalizer_x16_mono",
        "rvwk",
        LSP_GRAPH_EQUALIZER_BASE + 0,
        LSP_VERSION(1, 0, 2),
        graph_equalizer_classes,
        E_INLINE_DISPLAY,
        graph_equalizer_x16_mono_ports,
        "equalizer/graphic/x16/mono.xml",
        NULL,
        mono_plugin_port_groups
    };

    const plugin_metadata_t  graph_equalizer_x32_mono_metadata::metadata =
    {
        "Grafischer Entzerrer x32 Mono",
        "Graphic Equalizer x32 Mono",
        "GE32M",
        &developers::v_sadovnikov,
        "graph_equalizer_x32_mono",
        "vnca",
        LSP_GRAPH_EQUALIZER_BASE + 1,
        LSP_VERSION(1, 0, 2),
        graph_equalizer_classes,
        E_INLINE_DISPLAY,
        graph_equalizer_x32_mono_ports,
        "equalizer/graphic/x32/mono.xml",
        NULL,
        mono_plugin_port_groups
    };

    const plugin_metadata_t  graph_equalizer_x16_stereo_metadata::metadata =
    {
        "Grafischer Entzerrer x16 Stereo",
        "Graphic Equalizer x16 Stereo",
        "GE16S",
        &developers::v_sadovnikov,
        "graph_equalizer_x16_stereo",
        "argl",
        LSP_GRAPH_EQUALIZER_BASE + 2,
        LSP_VERSION(1, 0, 2),
        graph_equalizer_classes,
        E_INLINE_DISPLAY,
        graph_equalizer_x16_stereo_ports,
        "equalizer/graphic/x16/stereo.xml",
        NULL,
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  graph_equalizer_x32_stereo_metadata::metadata =
    {
        "Grafischer Entzerrer x32 Stereo",
        "Graphic Equalizer x32 Stereo",
        "GE32S",
        &developers::v_sadovnikov,
        "graph_equalizer_x32_stereo",
        "nvsd",
        LSP_GRAPH_EQUALIZER_BASE + 3,
        LSP_VERSION(1, 0, 2),
        graph_equalizer_classes,
        E_INLINE_DISPLAY,
        graph_equalizer_x32_stereo_ports,
        "equalizer/graphic/x32/stereo.xml",
        NULL,
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  graph_equalizer_x16_lr_metadata::metadata =
    {
        "Grafischer Entzerrer x16 LeftRight",
        "Graphic Equalizer x16 LeftRight",
        "GE16LR",
        &developers::v_sadovnikov,
        "graph_equalizer_x16_lr",
        "zefi",
        LSP_GRAPH_EQUALIZER_BASE + 4,
        LSP_VERSION(1, 0, 2),
        graph_equalizer_classes,
        E_INLINE_DISPLAY,
        graph_equalizer_x16_lr_ports,
        "equalizer/graphic/x16/lr.xml",
        NULL,
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  graph_equalizer_x32_lr_metadata::metadata =
    {
        "Grafischer Entzerrer x32 LeftRight",
        "Graphic Equalizer x32 LeftRight",
        "GE32LR",
        &developers::v_sadovnikov,
        "graph_equalizer_x32_lr",
        "0heu",
        LSP_GRAPH_EQUALIZER_BASE + 5,
        LSP_VERSION(1, 0, 2),
        graph_equalizer_classes,
        E_INLINE_DISPLAY,
        graph_equalizer_x32_lr_ports,
        "equalizer/graphic/x32/lr.xml",
        NULL,
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  graph_equalizer_x16_ms_metadata::metadata =
    {
        "Grafischer Entzerrer x16 MidSide",
        "Graphic Equalizer x16 MidSide",
        "GE16MS",
        &developers::v_sadovnikov,
        "graph_equalizer_x16_ms",
        "woys",
        LSP_GRAPH_EQUALIZER_BASE + 6,
        LSP_VERSION(1, 0, 2),
        graph_equalizer_classes,
        E_INLINE_DISPLAY,
        graph_equalizer_x16_ms_ports,
        "equalizer/graphic/x16/ms.xml",
        NULL,
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  graph_equalizer_x32_ms_metadata::metadata =
    {
        "Grafischer Entzerrer x32 MidSide",
        "Graphic Equalizer x32 MidSide",
        "GE32MS",
        &developers::v_sadovnikov,
        "graph_equalizer_x32_ms",
        "ku8j",
        LSP_GRAPH_EQUALIZER_BASE + 7,
        LSP_VERSION(1, 0, 2),
        graph_equalizer_classes,
        E_INLINE_DISPLAY,
        graph_equalizer_x32_ms_ports,
        "equalizer/graphic/x32/ms.xml",
        NULL,
        stereo_plugin_port_groups
    };
}
