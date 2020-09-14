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

    static const port_item_t crossover_selector_lr[] =
    {
        { "Left",           "crossover.sel.left"            },
        { "Right",          "crossover.sel.right"           },

        { NULL, NULL }
    };

    static const port_item_t crossover_selector_ms[] =
    {
        { "Middle",         "crossover.sel.mid"             },
        { "Side",           "crossover.sel.side"            },

        { NULL, NULL }
    };

    static const port_item_t crossover_slopes[] =
    {
        { "Off",            "crossover.slope.off"           },
        { "24 dB/oct",      "crossover.slope.24dbo"         },
        { "48 dB/oct",      "crossover.slope.48dbo"         },
        { "72 dB/oct",      "crossover.slope.72dbo"         },
        { "96 dB/oct",      "crossover.slope.96dbo"         },
        { NULL, NULL }
    };

    #define XOVER_COMMON \
            BYPASS, \
            AMP_GAIN("g_in", "Input gain", crossover_base_metadata::IN_GAIN_DFL, 10.0f), \
            AMP_GAIN("g_out", "Output gain", crossover_base_metadata::OUT_GAIN_DFL, 10.0f), \
            LOG_CONTROL("react", "FFT reactivity", U_MSEC, crossover_base_metadata::REACT_TIME), \
            AMP_GAIN("shift", "Shift gain", GAIN_AMP_0_DB, GAIN_AMP_P_60_DB), \
            LOG_CONTROL("zoom", "Graph zoom", U_GAIN_AMP, crossover_base_metadata::ZOOM)

    #define XOVER_CHANNEL(id, label) \
            SWITCH("flt" id, "Band filter curves" label, 1.0f), \
            SWITCH("crv" id, "Overall filter curve" label, 1.0f), \
            MESH("ag" id, "Amplitude graph" label, 2, crossover_base_metadata::MESH_POINTS)

    #define XOVER_FFT_METERS(id, label) \
            SWITCH("ife" id, "Input FFT graph enable" label, 1.0f), \
            SWITCH("ofe" id, "Output FFT graph enable" label, 1.0f), \
            MESH("ifg" id, "Input FFT graph" label, 2, crossover_base_metadata::MESH_POINTS), \
            MESH("ofg" id, "Output FFT graph" label, 2, crossover_base_metadata::MESH_POINTS)

    #define XOVER_CHANNEL_METERS(id, label) \
            METER_GAIN("ilm" id, "Input level meter" label, GAIN_AMP_P_24_DB), \
            METER_GAIN("olm" id, "Output level meter" label, GAIN_AMP_P_24_DB)

    #define XOVER_SPLIT(id, label, slope, freq) \
            COMBO("frs" id, "Frequency range slope" label, crossover_base_metadata::SLOPE_DFL * slope, crossover_slopes), \
            LOG_CONTROL_DFL("sf" id, "Split frequency" label, U_HZ, crossover_base_metadata::SPLIT_FREQ, freq)

    #define XOVER_BAND(id, label, x, total, fe, fs) \
            SWITCH("bs" id, "Solo band" label, 0.0f), \
            SWITCH("bm" id, "Mute band" label, 0.0f), \
            LOG_CONTROL("bg" id, "Band gain" label, U_GAIN_AMP, crossover_base_metadata::MAKEUP), \
            HUE_CTL("hue" id, "Hue " label, float(x) / float(total)), \
            METER("fre" id, "Frequency range end" label, U_HZ,  mb_compressor_base_metadata::OUT_FREQ), \
            MESH("bfc" id, "Band frequency chart" label, 2, crossover_base_metadata::FILTER_MESH_POINTS)

    #define XOVER_BAND_METER_MONO(id, label) \
            METER_GAIN("blm" id, "Band level meter" label, GAIN_AMP_P_24_DB)

    #define XOVER_BAND_METER_STEREO(id, label) \
            METER_GAIN("blm" id "l", "Band level meter" label " Left", GAIN_AMP_P_24_DB), \
            METER_GAIN("blm" id "r", "Band level meter" label " Right", GAIN_AMP_P_24_DB)

    #define XOVER_BAND_METER_MS(id, label) \
            METER_GAIN("blm" id "m", "Band level meter" label " Mid", GAIN_AMP_P_24_DB), \
            METER_GAIN("blm" id "s", "Band level meter" label " Side", GAIN_AMP_P_24_DB)

    #define XOVER_GROUP_PORTS(i) \
            MONO_PORT_GROUP_PORT(xover_pg_mono_ ## i, "band" #i); \
            STEREO_PORT_GROUP_PORTS(xover_pg_stereo_ ## i, "band" #i "l", "band" #i "r"); \
            MS_PORT_GROUP_PORTS(xover_pg_ms_ ## i, "band" #i "m", "band" #i "s");

    #define XOVER_MONO_GROUP(i) \
            { "mono_band" #i, "Mono band " #i " output",        GRP_MONO,       PGF_OUT,    xover_pg_mono_ ## i ##_ports        }

    #define XOVER_STEREO_GROUP(i) \
            { "stereo_band" #i, "Stereo band " #i " output",    GRP_STEREO,     PGF_OUT,    xover_pg_stereo_ ## i ##_ports      }

    #define XOVER_MS_GROUP(i) \
            { "ms_band" #i, "Mid/side band " #i " output",      GRP_MS,         PGF_OUT,    xover_pg_ms_ ## i ##_ports          }

    XOVER_GROUP_PORTS(0);
    XOVER_GROUP_PORTS(1);
    XOVER_GROUP_PORTS(2);
    XOVER_GROUP_PORTS(3);
    XOVER_GROUP_PORTS(4);
    XOVER_GROUP_PORTS(5);
    XOVER_GROUP_PORTS(6);
    XOVER_GROUP_PORTS(7);

    static const port_group_t xover_mono_port_groups[] =
    {
        MAIN_MONO_PORT_GROUPS,
        XOVER_MONO_GROUP(0),
        XOVER_MONO_GROUP(1),
        XOVER_MONO_GROUP(2),
        XOVER_MONO_GROUP(3),
        XOVER_MONO_GROUP(4),
        XOVER_MONO_GROUP(5),
        XOVER_MONO_GROUP(6),
        XOVER_MONO_GROUP(7),
        PORT_GROUPS_END
    };

    static const port_group_t xover_stereo_port_groups[] =
    {
        MAIN_STEREO_PORT_GROUPS,
        XOVER_STEREO_GROUP(0),
        XOVER_STEREO_GROUP(1),
        XOVER_STEREO_GROUP(2),
        XOVER_STEREO_GROUP(3),
        XOVER_STEREO_GROUP(4),
        XOVER_STEREO_GROUP(5),
        XOVER_STEREO_GROUP(6),
        XOVER_STEREO_GROUP(7),
        PORT_GROUPS_END
    };

    static const port_group_t xover_ms_port_groups[] =
    {
        MAIN_STEREO_PORT_GROUPS,
        XOVER_MS_GROUP(0),
        XOVER_MS_GROUP(1),
        XOVER_MS_GROUP(2),
        XOVER_MS_GROUP(3),
        XOVER_MS_GROUP(4),
        XOVER_MS_GROUP(5),
        XOVER_MS_GROUP(6),
        XOVER_MS_GROUP(7),
        PORT_GROUPS_END
    };

    static const port_t crossover_mono_ports[] =
    {
        PORTS_MONO_PLUGIN,
        AUDIO_OUTPUT("band0", "Band Output 0"),
        AUDIO_OUTPUT("band1", "Band Output 1"),
        AUDIO_OUTPUT("band2", "Band Output 2"),
        AUDIO_OUTPUT("band3", "Band Output 3"),
        AUDIO_OUTPUT("band4", "Band Output 4"),
        AUDIO_OUTPUT("band5", "Band Output 5"),
        AUDIO_OUTPUT("band6", "Band Output 6"),
        AUDIO_OUTPUT("band7", "Band Output 7"),

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

        XOVER_BAND_METER_MONO("_0", " 0"),
        XOVER_BAND_METER_MONO("_1", " 1"),
        XOVER_BAND_METER_MONO("_2", " 2"),
        XOVER_BAND_METER_MONO("_3", " 3"),
        XOVER_BAND_METER_MONO("_4", " 4"),
        XOVER_BAND_METER_MONO("_5", " 5"),
        XOVER_BAND_METER_MONO("_6", " 6"),
        XOVER_BAND_METER_MONO("_7", " 7"),

        PORTS_END
    };

    static const port_t crossover_stereo_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        AUDIO_OUTPUT("band0l", "Band Output 0 Left"),
        AUDIO_OUTPUT("band0r", "Band Output 0 Right"),
        AUDIO_OUTPUT("band1l", "Band Output 1 Left"),
        AUDIO_OUTPUT("band1r", "Band Output 1 Right"),
        AUDIO_OUTPUT("band2l", "Band Output 2 Left"),
        AUDIO_OUTPUT("band2r", "Band Output 2 Right"),
        AUDIO_OUTPUT("band3l", "Band Output 3 Left"),
        AUDIO_OUTPUT("band3r", "Band Output 3 Right"),
        AUDIO_OUTPUT("band4l", "Band Output 4 Left"),
        AUDIO_OUTPUT("band4r", "Band Output 4 Right"),
        AUDIO_OUTPUT("band5l", "Band Output 5 Left"),
        AUDIO_OUTPUT("band5r", "Band Output 5 Right"),
        AUDIO_OUTPUT("band6l", "Band Output 6 Left"),
        AUDIO_OUTPUT("band6r", "Band Output 6 Right"),
        AUDIO_OUTPUT("band7l", "Band Output 7 Left"),
        AUDIO_OUTPUT("band7r", "Band Output 7 Right"),

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

        XOVER_BAND("_0", " 0", 0, 8, 10.0f, 40.0f),
        XOVER_BAND("_1", " 1", 1, 8, 40.0f, 100.0f),
        XOVER_BAND("_2", " 2", 2, 8, 100.0f, 252.0f),
        XOVER_BAND("_3", " 3", 3, 8, 252.0f, 632.0f),
        XOVER_BAND("_4", " 4", 4, 8, 632.0f, 1587.0f),
        XOVER_BAND("_5", " 5", 5, 8, 1587.0f, 3984.0f),
        XOVER_BAND("_6", " 6", 6, 8, 3984.0f, 10000.0f),
        XOVER_BAND("_7", " 7", 7, 8, 10000.0f, 20000.0f),

        XOVER_BAND_METER_STEREO("_0", " 0"),
        XOVER_BAND_METER_STEREO("_1", " 1"),
        XOVER_BAND_METER_STEREO("_2", " 2"),
        XOVER_BAND_METER_STEREO("_3", " 3"),
        XOVER_BAND_METER_STEREO("_4", " 4"),
        XOVER_BAND_METER_STEREO("_5", " 5"),
        XOVER_BAND_METER_STEREO("_6", " 6"),
        XOVER_BAND_METER_STEREO("_7", " 7"),

        PORTS_END
    };

    static const port_t crossover_lr_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        AUDIO_OUTPUT("band0l", "Band Output 0 Left"),
        AUDIO_OUTPUT("band0r", "Band Output 0 Right"),
        AUDIO_OUTPUT("band1l", "Band Output 1 Left"),
        AUDIO_OUTPUT("band1r", "Band Output 1 Right"),
        AUDIO_OUTPUT("band2l", "Band Output 2 Left"),
        AUDIO_OUTPUT("band2r", "Band Output 2 Right"),
        AUDIO_OUTPUT("band3l", "Band Output 3 Left"),
        AUDIO_OUTPUT("band3r", "Band Output 3 Right"),
        AUDIO_OUTPUT("band4l", "Band Output 4 Left"),
        AUDIO_OUTPUT("band4r", "Band Output 4 Right"),
        AUDIO_OUTPUT("band5l", "Band Output 5 Left"),
        AUDIO_OUTPUT("band5r", "Band Output 5 Right"),
        AUDIO_OUTPUT("band6l", "Band Output 6 Left"),
        AUDIO_OUTPUT("band6r", "Band Output 6 Right"),
        AUDIO_OUTPUT("band7l", "Band Output 7 Left"),
        AUDIO_OUTPUT("band7r", "Band Output 7 Right"),

        XOVER_COMMON,
        COMBO("sel", "Processor selector", 0.0f, crossover_selector_lr),
        XOVER_CHANNEL("_l", " Left"),
        XOVER_CHANNEL("_r", " Right"),
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

        XOVER_BAND_METER_STEREO("_0", " 0"),
        XOVER_BAND_METER_STEREO("_1", " 1"),
        XOVER_BAND_METER_STEREO("_2", " 2"),
        XOVER_BAND_METER_STEREO("_3", " 3"),
        XOVER_BAND_METER_STEREO("_4", " 4"),
        XOVER_BAND_METER_STEREO("_5", " 5"),
        XOVER_BAND_METER_STEREO("_6", " 6"),
        XOVER_BAND_METER_STEREO("_7", " 7"),

        PORTS_END
    };

    static const port_t crossover_ms_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        AUDIO_OUTPUT("band0m", "Band Output 0 Mid"),
        AUDIO_OUTPUT("band0s", "Band Output 0 Side"),
        AUDIO_OUTPUT("band1m", "Band Output 1 Mid"),
        AUDIO_OUTPUT("band1s", "Band Output 1 Side"),
        AUDIO_OUTPUT("band2m", "Band Output 2 Mid"),
        AUDIO_OUTPUT("band2s", "Band Output 2 Side"),
        AUDIO_OUTPUT("band3m", "Band Output 3 Mid"),
        AUDIO_OUTPUT("band3s", "Band Output 3 Side"),
        AUDIO_OUTPUT("band4m", "Band Output 4 Mid"),
        AUDIO_OUTPUT("band4s", "Band Output 4 Side"),
        AUDIO_OUTPUT("band5m", "Band Output 5 Mid"),
        AUDIO_OUTPUT("band5s", "Band Output 5 Side"),
        AUDIO_OUTPUT("band6m", "Band Output 6 Mid"),
        AUDIO_OUTPUT("band6s", "Band Output 6 Side"),
        AUDIO_OUTPUT("band7m", "Band Output 7 Mid"),
        AUDIO_OUTPUT("band7s", "Band Output 7 Side"),

        XOVER_COMMON,
        COMBO("sel", "Processor selector", 0.0f, crossover_selector_ms),
        SWITCH("msout", "Mid/Side output", 0.0f),
        XOVER_CHANNEL("_m", " Mid"),
        XOVER_CHANNEL("_s", " Side"),
        XOVER_FFT_METERS("_m", " Mid"),
        XOVER_CHANNEL_METERS("_l", " Left"),
        XOVER_FFT_METERS("_s", " Mid"),
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

        XOVER_BAND_METER_MS("_0", " 0"),
        XOVER_BAND_METER_MS("_1", " 1"),
        XOVER_BAND_METER_MS("_2", " 2"),
        XOVER_BAND_METER_MS("_3", " 3"),
        XOVER_BAND_METER_MS("_4", " 4"),
        XOVER_BAND_METER_MS("_5", " 5"),
        XOVER_BAND_METER_MS("_6", " 6"),
        XOVER_BAND_METER_MS("_7", " 7"),

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
        "rmnv",
        LSP_CROSSOVER_BASE + 0,
        LSP_VERSION(1, 0, 0),
        crossover_classes,
        E_INLINE_DISPLAY | E_DUMP_STATE,
        crossover_mono_ports,
        "util/crossover/mono.xml",
        NULL,
        xover_mono_port_groups
    };

    const plugin_metadata_t  crossover_stereo_metadata::metadata =
    {
        "Frequenzweiche Stereo x8",
        "Crossover Stereo x8",
        "FW8S",
        &developers::v_sadovnikov,
        "crossover_stereo",
        "ooqb",
        LSP_CROSSOVER_BASE + 1,
        LSP_VERSION(1, 0, 0),
        crossover_classes,
        E_INLINE_DISPLAY | E_DUMP_STATE,
        crossover_stereo_ports,
        "util/crossover/stereo.xml",
        NULL,
        xover_stereo_port_groups
    };

    const plugin_metadata_t  crossover_lr_metadata::metadata =
    {
        "Frequenzweiche LeftRight x8",
        "Crossover LeftRight x8",
        "FW8LR",
        &developers::v_sadovnikov,
        "crossover_lr",
        "wvbr",
        LSP_CROSSOVER_BASE + 2,
        LSP_VERSION(1, 0, 0),
        crossover_classes,
        E_INLINE_DISPLAY | E_DUMP_STATE,
        crossover_lr_ports,
        "util/crossover/lr.xml",
        NULL,
        xover_stereo_port_groups
    };

    const plugin_metadata_t  crossover_ms_metadata::metadata =
    {
        "Frequenzweiche MidSide x8",
        "Crossover MidSide x8",
        "FW8LR",
        &developers::v_sadovnikov,
        "crossover_ms",
        "vlqv",
        LSP_CROSSOVER_BASE + 3,
        LSP_VERSION(1, 0, 0),
        crossover_classes,
        E_INLINE_DISPLAY | E_DUMP_STATE,
        crossover_ms_ports,
        "util/crossover/ms.xml",
        NULL,
        xover_ms_port_groups
    };
}


