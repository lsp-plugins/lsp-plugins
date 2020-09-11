/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 8 сент. 2020 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#include <core/types.h>

#include <metadata/plugins.h>
#include <metadata/ports.h>
#include <metadata/developers.h>

namespace lsp
{
    //-------------------------------------------------------------------------
    // Crossover plugin
    static const int crossover_classes[]        =   { C_UTILITY, -1 };

    static const port_item_t crossover_slopes[] =
    {
        { "off",            "crossover.slope.off"           },
        { "6 dB/oct",       "crossover.slope.6dbo"          },
        { "12 dB/oct",      "crossover.slope.12dbo"         },
        { "24 dB/oct",      "crossover.slope.24dbo"         },
        { "36 dB/oct",      "crossover.slope.36dbo"         },
        { "48 dB/oct",      "crossover.slope.48dbo"         },
        { "60 dB/oct",      "crossover.slope.60dbo"         },
        { "72 dB/oct",      "crossover.slope.72dbo"         },
        { NULL, NULL }
    };

    #define XOVER_COMMON \
            BYPASS, \
            AMP_GAIN("g_in", "Input gain", crossover_base_metadata::IN_GAIN_DFL, 10.0f), \
            AMP_GAIN("g_out", "Output gain", crossover_base_metadata::OUT_GAIN_DFL, 10.0f), \
            LOG_CONTROL("react", "FFT reactivity", U_MSEC, crossover_base_metadata::REACT_TIME), \
            AMP_GAIN("shift", "Shift gain", GAIN_AMP_M_48_DB, GAIN_AMP_P_48_DB), \
            LOG_CONTROL("zoom", "Graph zoom", U_GAIN_AMP, crossover_base_metadata::ZOOM)

    #define XOVER_CHANNEL(id, label) \
            SWITCH("flt" id, "Band filter curves" label, 1.0f), \
            MESH("ag" id, "Amplitude graph" label, 2, crossover_base_metadata::FILTER_MESH_POINTS)

    #define XOVER_FFT_METERS(id, label) \
            SWITCH("ife" id, "Input FFT graph enable" label, 1.0f), \
            SWITCH("ofe" id, "Output FFT graph enable" label, 1.0f), \
            MESH("ifg" id, "Input FFT graph" label, 2, crossover_base_metadata::MESH_POINTS), \
            MESH("ofg" id, "Output FFT graph" label, 2, crossover_base_metadata::MESH_POINTS)

    #define XOVER_CHANNEL_METERS(id, label) \
            METER_GAIN("ilm" id, "Input level meter" label, GAIN_AMP_P_24_DB), \
            METER_GAIN("olm" id, "Output level meter" label, GAIN_AMP_P_24_DB)

    #define XOVER_SPLIT(id, label, slope, freq) \
            LOG_CONTROL_DFL("sf" id, "Split frequency" label, U_HZ, crossover_base_metadata::SPLIT_FREQ, freq), \
            COMBO("frs", "Frequency range slope", crossover_base_metadata::SLOPE_DFL * slope, crossover_slopes)

    #define XOVER_BAND(id, label, x, total, fe, fs) \
            SWITCH("bs" id, "Solo band" label, 0.0f), \
            SWITCH("bm" id, "Mute band" label, 0.0f), \
            SWITCH("phi" id, "Band phase invert" label, 0.0f), \
            LOG_CONTROL("mk" id, "Makeup gain" label, U_GAIN_AMP, crossover_base_metadata::MAKEUP), \
            HUE_CTL("hue" id, "Hue " label, float(x) / float(total)), \
            METER("fre" id, "Frequency range end" label, U_HZ,  mb_compressor_base_metadata::OUT_FREQ), \
            MESH("bfc" id, "Band frequency chart" label, 2, crossover_base_metadata::FILTER_MESH_POINTS), \
            \
            METER_GAIN("ilm" id, "Input level meter" label, GAIN_AMP_P_24_DB)

    #define XOVER_STEREO_BAND(id, label, x, total, fe, fs) \
            SWITCH("bs" id, "Solo band" label, 0.0f), \
            SWITCH("bm" id, "Mute band" label, 0.0f), \
            SWITCH("phi" id, "Band phase invert" label, 0.0f), \
            LOG_CONTROL("mk" id, "Makeup gain" label, U_GAIN_AMP, crossover_base_metadata::MAKEUP), \
            HUE_CTL("hue" id, "Hue " label, float(x) / float(total)), \
            METER("fre" id, "Frequency range end" label, U_HZ,  mb_compressor_base_metadata::OUT_FREQ), \
            MESH("bag" id, "Band amplitude graph" label, 2, crossover_base_metadata::MESH_POINTS), \
            \
            METER_GAIN("ilm" id "l", "Input level meter" label " Left", GAIN_AMP_P_24_DB), \
            METER_GAIN("ilm" id "r", "Input level meter" label " Right", GAIN_AMP_P_24_DB)

    static const port_t crossover_mono_ports[] =
    {
        PORTS_MONO_PLUGIN,
        AUDIO_OUTPUT("0", "Band Output 0"),
        AUDIO_OUTPUT("1", "Band Output 1"),
        AUDIO_OUTPUT("2", "Band Output 2"),
        AUDIO_OUTPUT("3", "Band Output 3"),
        AUDIO_OUTPUT("4", "Band Output 4"),
        AUDIO_OUTPUT("5", "Band Output 5"),
        AUDIO_OUTPUT("6", "Band Output 6"),
        AUDIO_OUTPUT("7", "Band Output 7"),

        XOVER_COMMON,
        XOVER_CHANNEL("", ""),
        XOVER_FFT_METERS("", ""),
        XOVER_CHANNEL_METERS("", ""),

        XOVER_SPLIT("_1", " 1", 0, 40.0f),
        XOVER_SPLIT("_2", " 2", 1, 100.0f),
        XOVER_SPLIT("_3", " 3", 0, 252.0f),
        XOVER_SPLIT("_4", " 4", 1, 632.0f),
        XOVER_SPLIT("_5", " 5", 0, 1587.0f),
        XOVER_SPLIT("_6", " 6", 1, 3984.0f),
        XOVER_SPLIT("_7", " 7", 0, 10000.0f),

        XOVER_BAND("_0", " 0", 0, 8, 10.0f, 40.0f),
        XOVER_BAND("_1", " 1", 1, 8, 40.0f, 100.0f),
        XOVER_BAND("_2", " 2", 2, 8, 100.0f, 252.0f),
        XOVER_BAND("_3", " 3", 3, 8, 252.0f, 632.0f),
        XOVER_BAND("_4", " 4", 4, 8, 632.0f, 1587.0f),
        XOVER_BAND("_5", " 5", 5, 8, 1587.0f, 3984.0f),
        XOVER_BAND("_6", " 6", 6, 8, 3984.0f, 10000.0f),
        XOVER_BAND("_7", " 7", 7, 8, 10000.0f, 20000.0f),

        PORTS_END
    };

    static const port_t crossover_stereo_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        AUDIO_OUTPUT("0l", "Band Output 0 Left"),
        AUDIO_OUTPUT("0r", "Band Output 0 Right"),
        AUDIO_OUTPUT("1l", "Band Output 1 Left"),
        AUDIO_OUTPUT("1r", "Band Output 1 Right"),
        AUDIO_OUTPUT("2l", "Band Output 2 Left"),
        AUDIO_OUTPUT("2r", "Band Output 2 Right"),
        AUDIO_OUTPUT("3l", "Band Output 3 Left"),
        AUDIO_OUTPUT("3r", "Band Output 3 Right"),
        AUDIO_OUTPUT("4l", "Band Output 4 Left"),
        AUDIO_OUTPUT("4r", "Band Output 4 Right"),
        AUDIO_OUTPUT("5l", "Band Output 5 Left"),
        AUDIO_OUTPUT("5r", "Band Output 5 Right"),
        AUDIO_OUTPUT("6l", "Band Output 6 Left"),
        AUDIO_OUTPUT("6r", "Band Output 6 Right"),
        AUDIO_OUTPUT("7l", "Band Output 7 Left"),
        AUDIO_OUTPUT("7r", "Band Output 7 Right"),

        XOVER_COMMON,
        XOVER_CHANNEL("", ""),
        XOVER_FFT_METERS("_l", " Left"),
        XOVER_CHANNEL_METERS("_l", " Left"),
        XOVER_FFT_METERS("_r", " Right"),
        XOVER_CHANNEL_METERS("_r", " Right"),

        XOVER_SPLIT("_1", " 1", 0, 40.0f),
        XOVER_SPLIT("_2", " 2", 1, 100.0f),
        XOVER_SPLIT("_3", " 3", 0, 252.0f),
        XOVER_SPLIT("_4", " 4", 1, 632.0f),
        XOVER_SPLIT("_5", " 5", 0, 1587.0f),
        XOVER_SPLIT("_6", " 6", 1, 3984.0f),
        XOVER_SPLIT("_7", " 7", 0, 10000.0f),

        XOVER_STEREO_BAND("_0", " 0", 0, 8, 10.0f, 40.0f),
        XOVER_STEREO_BAND("_1", " 1", 1, 8, 40.0f, 100.0f),
        XOVER_STEREO_BAND("_2", " 2", 2, 8, 100.0f, 252.0f),
        XOVER_STEREO_BAND("_3", " 3", 3, 8, 252.0f, 632.0f),
        XOVER_STEREO_BAND("_4", " 4", 4, 8, 632.0f, 1587.0f),
        XOVER_STEREO_BAND("_5", " 5", 5, 8, 1587.0f, 3984.0f),
        XOVER_STEREO_BAND("_6", " 6", 6, 8, 3984.0f, 10000.0f),
        XOVER_STEREO_BAND("_7", " 7", 7, 8, 10000.0f, 20000.0f),

        PORTS_END
    };

    static const port_t crossover_lr_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        AUDIO_OUTPUT("0l", "Band Output 0 Left"),
        AUDIO_OUTPUT("0r", "Band Output 0 Right"),
        AUDIO_OUTPUT("1l", "Band Output 1 Left"),
        AUDIO_OUTPUT("1r", "Band Output 1 Right"),
        AUDIO_OUTPUT("2l", "Band Output 2 Left"),
        AUDIO_OUTPUT("2r", "Band Output 2 Right"),
        AUDIO_OUTPUT("3l", "Band Output 3 Left"),
        AUDIO_OUTPUT("3r", "Band Output 3 Right"),
        AUDIO_OUTPUT("4l", "Band Output 4 Left"),
        AUDIO_OUTPUT("4r", "Band Output 4 Right"),
        AUDIO_OUTPUT("5l", "Band Output 5 Left"),
        AUDIO_OUTPUT("5r", "Band Output 5 Right"),
        AUDIO_OUTPUT("6l", "Band Output 6 Left"),
        AUDIO_OUTPUT("6r", "Band Output 6 Right"),
        AUDIO_OUTPUT("7l", "Band Output 7 Left"),
        AUDIO_OUTPUT("7r", "Band Output 7 Right"),

        XOVER_COMMON,
        XOVER_CHANNEL("", ""),
        XOVER_FFT_METERS("_l", " Left"),
        XOVER_CHANNEL_METERS("_l", " Left"),
        XOVER_FFT_METERS("_r", " Right"),
        XOVER_CHANNEL_METERS("_r", " Right"),

        XOVER_SPLIT("_1l", " 1 Left", 0, 40.0f),
        XOVER_SPLIT("_2l", " 2 Left", 1, 100.0f),
        XOVER_SPLIT("_3l", " 3 Left", 0, 252.0f),
        XOVER_SPLIT("_4l", " 4 Left", 1, 632.0f),
        XOVER_SPLIT("_5l", " 5 Left", 0, 1587.0f),
        XOVER_SPLIT("_6l", " 6 Left", 1, 3984.0f),
        XOVER_SPLIT("_7l", " 7 Left", 0, 10000.0f),

        XOVER_SPLIT("_1r", " 1 Right", 0, 40.0f),
        XOVER_SPLIT("_2r", " 2 Right", 1, 100.0f),
        XOVER_SPLIT("_3r", " 3 Right", 0, 252.0f),
        XOVER_SPLIT("_4r", " 4 Right", 1, 632.0f),
        XOVER_SPLIT("_5r", " 5 Right", 0, 1587.0f),
        XOVER_SPLIT("_6r", " 6 Right", 1, 3984.0f),
        XOVER_SPLIT("_7r", " 7 Right", 0, 10000.0f),

        XOVER_BAND("_0l", " 0 Left", 0, 8, 10.0f, 40.0f),
        XOVER_BAND("_1l", " 1 Left", 1, 8, 40.0f, 100.0f),
        XOVER_BAND("_2l", " 2 Left", 2, 8, 100.0f, 252.0f),
        XOVER_BAND("_3l", " 3 Left", 3, 8, 252.0f, 632.0f),
        XOVER_BAND("_4l", " 4 Left", 4, 8, 632.0f, 1587.0f),
        XOVER_BAND("_5l", " 5 Left", 5, 8, 1587.0f, 3984.0f),
        XOVER_BAND("_6l", " 6 Left", 6, 8, 3984.0f, 10000.0f),
        XOVER_BAND("_7l", " 7 Left", 7, 8, 10000.0f, 20000.0f),

        XOVER_BAND("_0r", " 0 Right", 0, 8, 10.0f, 40.0f),
        XOVER_BAND("_1r", " 1 Right", 1, 8, 40.0f, 100.0f),
        XOVER_BAND("_2r", " 2 Right", 2, 8, 100.0f, 252.0f),
        XOVER_BAND("_3r", " 3 Right", 3, 8, 252.0f, 632.0f),
        XOVER_BAND("_4r", " 4 Right", 4, 8, 632.0f, 1587.0f),
        XOVER_BAND("_5r", " 5 Right", 5, 8, 1587.0f, 3984.0f),
        XOVER_BAND("_6r", " 6 Right", 6, 8, 3984.0f, 10000.0f),
        XOVER_BAND("_7r", " 7 Right", 7, 8, 10000.0f, 20000.0f),

        PORTS_END
    };

    static const port_t crossover_ms_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        AUDIO_OUTPUT("0m", "Band Output 0 Mid"),
        AUDIO_OUTPUT("0s", "Band Output 0 Side"),
        AUDIO_OUTPUT("1m", "Band Output 1 Mid"),
        AUDIO_OUTPUT("1s", "Band Output 1 Side"),
        AUDIO_OUTPUT("2m", "Band Output 2 Mid"),
        AUDIO_OUTPUT("2s", "Band Output 2 Side"),
        AUDIO_OUTPUT("3m", "Band Output 3 Mid"),
        AUDIO_OUTPUT("3s", "Band Output 3 Side"),
        AUDIO_OUTPUT("4m", "Band Output 4 Mid"),
        AUDIO_OUTPUT("4s", "Band Output 4 Side"),
        AUDIO_OUTPUT("5m", "Band Output 5 Mid"),
        AUDIO_OUTPUT("5s", "Band Output 5 Side"),
        AUDIO_OUTPUT("6m", "Band Output 6 Mid"),
        AUDIO_OUTPUT("6s", "Band Output 6 Side"),
        AUDIO_OUTPUT("7m", "Band Output 7 Mid"),
        AUDIO_OUTPUT("7s", "Band Output 7 Side"),

        XOVER_COMMON,
        SWITCH("msout", "Mid/Side output", 0.0f),
        XOVER_CHANNEL("", ""),
        XOVER_FFT_METERS("_l", " Left"),
        XOVER_CHANNEL_METERS("_l", " Left"),
        XOVER_FFT_METERS("_r", " Right"),
        XOVER_CHANNEL_METERS("_r", " Right"),

        XOVER_SPLIT("_1m", " 1 Mid", 0, 40.0f),
        XOVER_SPLIT("_2m", " 2 Mid", 1, 100.0f),
        XOVER_SPLIT("_3m", " 3 Mid", 0, 252.0f),
        XOVER_SPLIT("_4m", " 4 Mid", 1, 632.0f),
        XOVER_SPLIT("_5m", " 5 Mid", 0, 1587.0f),
        XOVER_SPLIT("_6m", " 6 Mid", 1, 3984.0f),
        XOVER_SPLIT("_7m", " 7 Mid", 0, 10000.0f),

        XOVER_SPLIT("_1s", " 1 Side", 0, 40.0f),
        XOVER_SPLIT("_2s", " 2 Side", 1, 100.0f),
        XOVER_SPLIT("_3s", " 3 Side", 0, 252.0f),
        XOVER_SPLIT("_4s", " 4 Side", 1, 632.0f),
        XOVER_SPLIT("_5s", " 5 Side", 0, 1587.0f),
        XOVER_SPLIT("_6s", " 6 Side", 1, 3984.0f),
        XOVER_SPLIT("_7s", " 7 Side", 0, 10000.0f),

        XOVER_BAND("_0m", " 0 Mid", 0, 8, 10.0f, 40.0f),
        XOVER_BAND("_1m", " 1 Mid", 1, 8, 40.0f, 100.0f),
        XOVER_BAND("_2m", " 2 Mid", 2, 8, 100.0f, 252.0f),
        XOVER_BAND("_3m", " 3 Mid", 3, 8, 252.0f, 632.0f),
        XOVER_BAND("_4m", " 4 Mid", 4, 8, 632.0f, 1587.0f),
        XOVER_BAND("_5m", " 5 Mid", 5, 8, 1587.0f, 3984.0f),
        XOVER_BAND("_6m", " 6 Mid", 6, 8, 3984.0f, 10000.0f),
        XOVER_BAND("_7m", " 7 Mid", 7, 8, 10000.0f, 20000.0f),

        XOVER_BAND("_0s", " 0 Side", 0, 8, 10.0f, 40.0f),
        XOVER_BAND("_1s", " 1 Side", 1, 8, 40.0f, 100.0f),
        XOVER_BAND("_2s", " 2 Side", 2, 8, 100.0f, 252.0f),
        XOVER_BAND("_3s", " 3 Side", 3, 8, 252.0f, 632.0f),
        XOVER_BAND("_4s", " 4 Side", 4, 8, 632.0f, 1587.0f),
        XOVER_BAND("_5s", " 5 Side", 5, 8, 1587.0f, 3984.0f),
        XOVER_BAND("_6s", " 6 Side", 6, 8, 3984.0f, 10000.0f),
        XOVER_BAND("_7s", " 7 Side", 7, 8, 10000.0f, 20000.0f),

        PORTS_END
    };

    // Crossover
    const plugin_metadata_t  crossover_mono_metadata::metadata =
    {
        "Frequenzweiche Mono x8",
        "Crossover Mono x8",
        "FW8M",
        &developers::v_sadovnikov,
        "crossover_mono",
        "----",
        LSP_CROSSOVER_BASE + 0,
        LSP_VERSION(1, 0, 0),
        crossover_classes,
        E_INLINE_DISPLAY | E_DUMP_STATE,
        crossover_mono_ports,
        "util/crossover/mono.xml",
        NULL,
        mono_plugin_port_groups
    };

    const plugin_metadata_t  crossover_stereo_metadata::metadata =
    {
        "Frequenzweiche Stereo x8",
        "Crossover Stereo x8",
        "FW8S",
        &developers::v_sadovnikov,
        "crossover_stereo",
        "----",
        LSP_CROSSOVER_BASE + 1,
        LSP_VERSION(1, 0, 0),
        crossover_classes,
        E_INLINE_DISPLAY | E_DUMP_STATE,
        crossover_stereo_ports,
        "util/crossover/stereo.xml",
        NULL,
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  crossover_lr_metadata::metadata =
    {
        "Frequenzweiche LeftRight x8",
        "Crossover LeftRight x8",
        "FW8LR",
        &developers::v_sadovnikov,
        "crossover_lr",
        "----",
        LSP_CROSSOVER_BASE + 2,
        LSP_VERSION(1, 0, 0),
        crossover_classes,
        E_INLINE_DISPLAY | E_DUMP_STATE,
        crossover_lr_ports,
        "util/crossover/lr.xml",
        NULL,
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  crossover_ms_metadata::metadata =
    {
        "Frequenzweiche MidSide x8",
        "Crossover MidSide x8",
        "FW8LR",
        &developers::v_sadovnikov,
        "crossover_ms",
        "----",
        LSP_CROSSOVER_BASE + 3,
        LSP_VERSION(1, 0, 0),
        crossover_classes,
        E_INLINE_DISPLAY | E_DUMP_STATE,
        crossover_ms_ports,
        "util/crossover/ms.xml",
        NULL,
        stereo_plugin_port_groups
    };
}


