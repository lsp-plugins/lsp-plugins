/*
 * mb_expander.cpp
 *
 *  Created on: 27 дек. 2019 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <core/util/Sidechain.h>

#include <metadata/plugins.h>
#include <metadata/ports.h>
#include <metadata/developers.h>

namespace lsp
{
    //-------------------------------------------------------------------------
    // Multiband expander
    static const int mb_expander_classes[] = { C_EXPANDER, -1 };

    static const port_item_t mb_exp_sc_modes[] =
    {
        { "Peak",       "sidechain.peak"           },
        { "RMS",        "sidechain.rms"            },
        { "Low-Pass",   "sidechain.lowpass"        },
        { "Uniform",    "sidechain.uniform"        },
        { NULL, NULL }
    };

    static const port_item_t mb_exp_sc_source[] =
    {
        { "Middle",     "sidechain.middle" },
        { "Side",       "sidechain.side" },
        { "Left",       "sidechain.left" },
        { "Right",      "sidechain.right" },
        { NULL, NULL }
    };

    static const port_item_t mb_exp_sc_boost[] =
    {
        { "None",       "sidechain.boost.none" },
        { "Pink BT",    "sidechain.boost.pink_bt" },
        { "Pink MT",    "sidechain.boost.pink_mt" },
        { "Brown BT",   "sidechain.boost.brown_bt" },
        { "Brown MT",   "sidechain.boost.brown_mt" },
        { NULL, NULL }
    };

    static const port_item_t mb_global_mb_exp_modes[] =
    {
        { "Classic",    "multiband.classic" },
        { "Modern",     "multiband.modern" },
        { NULL, NULL }
    };

    static const port_item_t mb_exp_modes[] =
    {
        { "Down",       "expander.down_ward" },
        { "Up",         "expander.up_ward" },
        { NULL, NULL }
    };

    static const port_item_t exp_sc_bands[] =
    {
        { "Split",          "mb_expand.split" },
        { "Band 0",         "mb_expand.band0" },
        { "Band 1",         "mb_expand.band1" },
        { "Band 2",         "mb_expand.band2" },
        { "Band 3",         "mb_expand.band3" },
        { "Band 4",         "mb_expand.band4" },
        { "Band 5",         "mb_expand.band5" },
        { "Band 6",         "mb_expand.band6" },
        { "Band 7",         "mb_expand.band7" },
        { NULL, NULL }
    };

    static const port_item_t exp_sc_lr_bands[] =
    {
        { "Split Left",     "mb_expand.split_left" },
        { "Split Right",    "mb_expand.split_right" },
        { "Band 0",         "mb_expand.band0" },
        { "Band 1",         "mb_expand.band1" },
        { "Band 2",         "mb_expand.band2" },
        { "Band 3",         "mb_expand.band3" },
        { "Band 4",         "mb_expand.band4" },
        { "Band 5",         "mb_expand.band5" },
        { "Band 6",         "mb_expand.band6" },
        { "Band 7",         "mb_expand.band7" },
        { NULL, NULL }
    };

    static const port_item_t exp_sc_ms_bands[] =
    {
        { "Split Mid",      "mb_expand.split_middle" },
        { "Split Side",     "mb_expand.split_side" },
        { "Band 0",         "mb_expand.band0" },
        { "Band 1",         "mb_expand.band1" },
        { "Band 2",         "mb_expand.band2" },
        { "Band 3",         "mb_expand.band3" },
        { "Band 4",         "mb_expand.band4" },
        { "Band 5",         "mb_expand.band5" },
        { "Band 6",         "mb_expand.band6" },
        { "Band 7",         "mb_expand.band7" },
        { NULL, NULL }
    };

    #define MB_COMMON(bands) \
        BYPASS, \
        COMBO("mode", "Expander mode", 1, mb_global_mb_exp_modes), \
        AMP_GAIN("g_in", "Input gain", mb_expander_base_metadata::IN_GAIN_DFL, 10.0f), \
        AMP_GAIN("g_out", "Output gain", mb_expander_base_metadata::OUT_GAIN_DFL, 10.0f), \
        AMP_GAIN("g_dry", "Dry gain", 0.0f, 10.0f), \
        AMP_GAIN("g_wet", "Wet gain", 1.0f, 10.0f), \
        LOG_CONTROL("react", "FFT reactivity", U_MSEC, mb_expander_base_metadata::FFT_REACT_TIME), \
        AMP_GAIN("shift", "Shift gain", 1.0f, 100.0f), \
        LOG_CONTROL("zoom", "Graph zoom", U_GAIN_AMP, mb_expander_base_metadata::ZOOM), \
        COMBO("envb", "Envelope boost", mb_expander_base_metadata::FB_DEFAULT, mb_exp_sc_boost), \
        COMBO("bsel", "Band selection", mb_expander_base_metadata::SC_BAND_DFL, bands)

    #define MB_CHANNEL(id, label) \
        SWITCH("flt" id, "Band filter curves" label, 1.0f), \
        MESH("ag" id, "Expander amplitude graph " label, 2, mb_expander_base_metadata::FFT_MESH_POINTS)

    #define MB_FFT_METERS(id, label) \
        SWITCH("ife" id, "Input FFT graph enable" label, 1.0f), \
        SWITCH("ofe" id, "Output FFT graph enable" label, 1.0f), \
        MESH("ifg" id, "Input FFT graph" label, 2, mb_expander_base_metadata::FFT_MESH_POINTS), \
        MESH("ofg" id, "Output FFT graph" label, 2, mb_expander_base_metadata::FFT_MESH_POINTS)

    #define MB_CHANNEL_METERS(id, label) \
        METER_GAIN("ilm" id, "Input level meter" label, GAIN_AMP_P_24_DB), \
        METER_GAIN("olm" id, "Output level meter" label, GAIN_AMP_P_24_DB)

    #define MB_SPLIT(id, label, enable, freq) \
        SWITCH("cbe" id, "Expander band enable" label, enable), \
        LOG_CONTROL_DFL("sf" id, "Split frequency" label, U_HZ, mb_expander_base_metadata::FREQ, freq)

    #define MB_MONO_BAND(id, label, x, total, fe, fs) \
        COMBO("scm" id, "Sidechain mode" label, mb_expander_base_metadata::SC_MODE_DFL, mb_exp_sc_modes), \
        CONTROL("sla" id, "Sidechain lookahead" label, U_MSEC, mb_expander_base_metadata::LOOKAHEAD), \
        LOG_CONTROL("scr" id, "Sidechain reactivity" label, U_MSEC, mb_expander_base_metadata::REACTIVITY), \
        AMP_GAIN100("scp" id, "Sidechain preamp" label, GAIN_AMP_0_DB), \
        SWITCH("sclc" id, "Sidechain custom lo-cut" label, 0), \
        SWITCH("schc" id, "Sidechain custom hi-cut" label, 0), \
        LOG_CONTROL_DFL("sclf" id, "Sidechain lo-cut frequency" label, U_HZ, mb_expander_base_metadata::FREQ, fe), \
        LOG_CONTROL_DFL("schf" id, "Sidechain hi-cut frequency" label, U_HZ, mb_expander_base_metadata::FREQ, fs), \
        MESH("bfc" id, "Side-chain band frequency chart" label, 2, mb_expander_base_metadata::FILTER_MESH_POINTS), \
        \
        COMBO("em" id, "Expander mode" label, mb_expander_base_metadata::EM_DEFAULT, mb_exp_modes), \
        SWITCH("ee" id, "Expander enable" label, 1.0f), \
        SWITCH("bs" id, "Solo band" label, 0.0f), \
        SWITCH("bm" id, "Mute band" label, 0.0f), \
        LOG_CONTROL("al" id, "Attack level" label, U_GAIN_AMP, mb_expander_base_metadata::ATTACK_LVL), \
        LOG_CONTROL("at" id, "Attack time" label, U_MSEC, mb_expander_base_metadata::ATTACK_TIME), \
        LOG_CONTROL("rrl" id, "Relative release level" label, U_GAIN_AMP, mb_expander_base_metadata::RELEASE_LVL), \
        LOG_CONTROL("rt" id, "Release time" label, U_MSEC, mb_expander_base_metadata::RELEASE_TIME), \
        LOG_CONTROL("er" id, "Ratio" label, U_NONE, mb_expander_base_metadata::RATIO), \
        LOG_CONTROL("kn" id, "Knee" label, U_GAIN_AMP, mb_expander_base_metadata::KNEE), \
        LOG_CONTROL("mk" id, "Makeup gain" label, U_GAIN_AMP, mb_expander_base_metadata::MAKEUP), \
        HUE_CTL("hue" id, "Hue " label, (float(x) / float(total))), \
        METER("fre" id, "Frequency range end" label, U_HZ,  mb_expander_base_metadata::OUT_FREQ), \
        MESH("ccg" id, "Expander curve graph" label, 2, mb_expander_base_metadata::CURVE_MESH_SIZE), \
        METER_OUT_GAIN("rl" id, "Release level" label, 20.0f), \
        METER_OUT_GAIN("elm" id, "Envelope level meter" label, GAIN_AMP_P_24_DB), \
        METER_OUT_GAIN("clm" id, "Curve level meter" label, GAIN_AMP_P_24_DB), \
        METER_OUT_GAIN("rlm" id, "Reduction level meter" label, GAIN_AMP_P_24_DB)

    #define MB_STEREO_BAND(id, label, x, total, fe, fs) \
        COMBO("scs" id, "Sidechain source" label, SCS_MIDDLE, mb_exp_sc_source), \
        MB_MONO_BAND(id, label, x, total, fe, fs)

    #define MB_SC_MONO_BAND(id, label, x, total, fe, fs) \
        SWITCH("sce" id, "External sidechain enable" label, 0.0f), \
        MB_MONO_BAND(id, label, x, total, fe, fs)

    #define MB_SC_STEREO_BAND(id, label, x, total, fe, fs) \
        SWITCH("sce" id, "External sidechain enable" label, 0.0f), \
        MB_STEREO_BAND(id, label, x, total, fe, fs)

    static const port_t mb_expander_mono_ports[] =
    {
        PORTS_MONO_PLUGIN,
        MB_COMMON(exp_sc_bands),
        MB_CHANNEL("", ""),
        MB_FFT_METERS("", ""),
        MB_CHANNEL_METERS("", ""),

        MB_SPLIT("_1", " 1", 0.0f, 40.0f),
        MB_SPLIT("_2", " 2", 1.0f, 100.0f),
        MB_SPLIT("_3", " 3", 0.0f, 252.0f),
        MB_SPLIT("_4", " 4", 1.0f, 632.0f),
        MB_SPLIT("_5", " 5", 0.0f, 1587.0f),
        MB_SPLIT("_6", " 6", 1.0f, 3984.0f),
        MB_SPLIT("_7", " 7", 0.0f, 10000.0f),

        MB_MONO_BAND("_0", " 0", 0, 8, 10.0f, 40.0f),
        MB_MONO_BAND("_1", " 1", 1, 8, 40.0f, 100.0f),
        MB_MONO_BAND("_2", " 2", 2, 8, 100.0f, 252.0f),
        MB_MONO_BAND("_3", " 3", 3, 8, 252.0f, 632.0f),
        MB_MONO_BAND("_4", " 4", 4, 8, 632.0f, 1587.0f),
        MB_MONO_BAND("_5", " 5", 5, 8, 1587.0f, 3984.0f),
        MB_MONO_BAND("_6", " 6", 6, 8, 3984.0f, 10000.0f),
        MB_MONO_BAND("_7", " 7", 7, 8, 10000.0f, 20000.0f),

        PORTS_END
    };

    static const port_t mb_expander_stereo_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        MB_COMMON(exp_sc_bands),
        MB_CHANNEL("", ""),
        MB_FFT_METERS("_l", " Left"),
        MB_CHANNEL_METERS("_l", " Left"),
        MB_FFT_METERS("_r", " Right"),
        MB_CHANNEL_METERS("_r", " Right"),

        MB_SPLIT("_1", " 1", 0.0f, 40.0f),
        MB_SPLIT("_2", " 2", 1.0f, 100.0f),
        MB_SPLIT("_3", " 3", 0.0f, 252.0f),
        MB_SPLIT("_4", " 4", 1.0f, 632.0f),
        MB_SPLIT("_5", " 5", 0.0f, 1587.0f),
        MB_SPLIT("_6", " 6", 1.0f, 3984.0f),
        MB_SPLIT("_7", " 7", 0.0f, 10000.0f),

        MB_STEREO_BAND("_0", " 0", 0, 8, 10.0f, 40.0f),
        MB_STEREO_BAND("_1", " 1", 1, 8, 40.0f, 100.0f),
        MB_STEREO_BAND("_2", " 2", 2, 8, 100.0f, 252.0f),
        MB_STEREO_BAND("_3", " 3", 3, 8, 252.0f, 632.0f),
        MB_STEREO_BAND("_4", " 4", 4, 8, 632.0f, 1587.0f),
        MB_STEREO_BAND("_5", " 5", 5, 8, 1587.0f, 3984.0f),
        MB_STEREO_BAND("_6", " 6", 6, 8, 3984.0f, 10000.0f),
        MB_STEREO_BAND("_7", " 7", 7, 8, 10000.0f, 20000.0f),

        PORTS_END
    };

    static const port_t mb_expander_lr_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        MB_COMMON(exp_sc_lr_bands),
        MB_CHANNEL("_l", " Left"),
        MB_CHANNEL("_r", " Right"),
        MB_FFT_METERS("_l", " Left"),
        MB_CHANNEL_METERS("_l", " Left"),
        MB_FFT_METERS("_r", " Right"),
        MB_CHANNEL_METERS("_r", " Right"),

        MB_SPLIT("_1l", " 1 Left", 0.0f, 40.0f),
        MB_SPLIT("_2l", " 2 Left", 1.0f, 100.0f),
        MB_SPLIT("_3l", " 3 Left", 0.0f, 252.0f),
        MB_SPLIT("_4l", " 4 Left", 1.0f, 632.0f),
        MB_SPLIT("_5l", " 5 Left", 0.0f, 1587.0f),
        MB_SPLIT("_6l", " 6 Left", 1.0f, 3984.0f),
        MB_SPLIT("_7l", " 7 Left", 0.0f, 10000.0f),

        MB_SPLIT("_1r", " 1 Right", 0.0f, 40.0f),
        MB_SPLIT("_2r", " 2 Right", 1.0f, 100.0f),
        MB_SPLIT("_3r", " 3 Right", 0.0f, 252.0f),
        MB_SPLIT("_4r", " 4 Right", 1.0f, 632.0f),
        MB_SPLIT("_5r", " 5 Right", 0.0f, 1587.0f),
        MB_SPLIT("_6r", " 6 Right", 1.0f, 3984.0f),
        MB_SPLIT("_7r", " 7 Right", 0.0f, 10000.0f),

        MB_STEREO_BAND("_0l", " 0 Left", 0, 8, 10.0f, 40.0f),
        MB_STEREO_BAND("_1l", " 1 Left", 1, 8, 40.0f, 100.0f),
        MB_STEREO_BAND("_2l", " 2 Left", 2, 8, 100.0f, 252.0f),
        MB_STEREO_BAND("_3l", " 3 Left", 3, 8, 252.0f, 632.0f),
        MB_STEREO_BAND("_4l", " 4 Left", 4, 8, 632.0f, 1587.0f),
        MB_STEREO_BAND("_5l", " 5 Left", 5, 8, 1587.0f, 3984.0f),
        MB_STEREO_BAND("_6l", " 6 Left", 6, 8, 3984.0f, 10000.0f),
        MB_STEREO_BAND("_7l", " 7 Left", 7, 8, 10000.0f, 20000.0f),

        MB_STEREO_BAND("_0r", " 0 Right", 0, 8, 10.0f, 40.0f),
        MB_STEREO_BAND("_1r", " 1 Right", 1, 8, 40.0f, 100.0f),
        MB_STEREO_BAND("_2r", " 2 Right", 2, 8, 100.0f, 252.0f),
        MB_STEREO_BAND("_3r", " 3 Right", 3, 8, 252.0f, 632.0f),
        MB_STEREO_BAND("_4r", " 4 Right", 4, 8, 632.0f, 1587.0f),
        MB_STEREO_BAND("_5r", " 5 Right", 5, 8, 1587.0f, 3984.0f),
        MB_STEREO_BAND("_6r", " 6 Right", 6, 8, 3984.0f, 10000.0f),
        MB_STEREO_BAND("_7r", " 7 Right", 7, 8, 10000.0f, 20000.0f),

        PORTS_END
    };

    static const port_t mb_expander_ms_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        MB_COMMON(exp_sc_ms_bands),
        MB_CHANNEL("_m", " Mid"),
        MB_CHANNEL("_s", " Side"),
        MB_FFT_METERS("_m", " Mid"),
        MB_CHANNEL_METERS("_l", " Left"),
        MB_FFT_METERS("_s", " Side"),
        MB_CHANNEL_METERS("_r", " Right"),

        MB_SPLIT("_1m", " 1 Mid", 0.0f, 40.0f),
        MB_SPLIT("_2m", " 2 Mid", 1.0f, 100.0f),
        MB_SPLIT("_3m", " 3 Mid", 0.0f, 252.0f),
        MB_SPLIT("_4m", " 4 Mid", 1.0f, 632.0f),
        MB_SPLIT("_5m", " 5 Mid", 0.0f, 1587.0f),
        MB_SPLIT("_6m", " 6 Mid", 1.0f, 3984.0f),
        MB_SPLIT("_7m", " 7 Mid", 0.0f, 10000.0f),

        MB_SPLIT("_1s", " 1 Side", 0.0f, 40.0f),
        MB_SPLIT("_2s", " 2 Side", 1.0f, 100.0f),
        MB_SPLIT("_3s", " 3 Side", 0.0f, 252.0f),
        MB_SPLIT("_4s", " 4 Side", 1.0f, 632.0f),
        MB_SPLIT("_5s", " 5 Side", 0.0f, 1587.0f),
        MB_SPLIT("_6s", " 6 Side", 1.0f, 3984.0f),
        MB_SPLIT("_7s", " 7 Side", 0.0f, 10000.0f),

        MB_STEREO_BAND("_0m", " 0 Mid", 0, 8, 10.0f, 40.0f),
        MB_STEREO_BAND("_1m", " 1 Mid", 1, 8, 40.0f, 100.0f),
        MB_STEREO_BAND("_2m", " 2 Mid", 2, 8, 100.0f, 252.0f),
        MB_STEREO_BAND("_3m", " 3 Mid", 3, 8, 252.0f, 632.0f),
        MB_STEREO_BAND("_4m", " 4 Mid", 4, 8, 632.0f, 1587.0f),
        MB_STEREO_BAND("_5m", " 5 Mid", 5, 8, 1587.0f, 3984.0f),
        MB_STEREO_BAND("_6m", " 6 Mid", 6, 8, 3984.0f, 10000.0f),
        MB_STEREO_BAND("_7m", " 7 Mid", 7, 8, 10000.0f, 20000.0f),

        MB_STEREO_BAND("_0s", " 0 Side", 0, 8, 10.0f, 40.0f),
        MB_STEREO_BAND("_1s", " 1 Side", 1, 8, 40.0f, 100.0f),
        MB_STEREO_BAND("_2s", " 2 Side", 2, 8, 100.0f, 252.0f),
        MB_STEREO_BAND("_3s", " 3 Side", 3, 8, 252.0f, 632.0f),
        MB_STEREO_BAND("_4s", " 4 Side", 4, 8, 632.0f, 1587.0f),
        MB_STEREO_BAND("_5s", " 5 Side", 5, 8, 1587.0f, 3984.0f),
        MB_STEREO_BAND("_6s", " 6 Side", 6, 8, 3984.0f, 10000.0f),
        MB_STEREO_BAND("_7s", " 7 Side", 7, 8, 10000.0f, 20000.0f),

        PORTS_END
    };

    static const port_t sc_mb_expander_mono_ports[] =
    {
        PORTS_MONO_PLUGIN,
        PORTS_MONO_SIDECHAIN,
        MB_COMMON(exp_sc_bands),
        MB_CHANNEL("", ""),
        MB_FFT_METERS("", ""),
        MB_CHANNEL_METERS("", ""),

        MB_SPLIT("_1", " 1", 0.0f, 40.0f),
        MB_SPLIT("_2", " 2", 1.0f, 100.0f),
        MB_SPLIT("_3", " 3", 0.0f, 252.0f),
        MB_SPLIT("_4", " 4", 1.0f, 632.0f),
        MB_SPLIT("_5", " 5", 0.0f, 1587.0f),
        MB_SPLIT("_6", " 6", 1.0f, 3984.0f),
        MB_SPLIT("_7", " 7", 0.0f, 10000.0f),

        MB_SC_MONO_BAND("_0", " 0", 0, 8, 10.0f, 40.0f),
        MB_SC_MONO_BAND("_1", " 1", 1, 8, 40.0f, 100.0f),
        MB_SC_MONO_BAND("_2", " 2", 2, 8, 100.0f, 252.0f),
        MB_SC_MONO_BAND("_3", " 3", 3, 8, 252.0f, 632.0f),
        MB_SC_MONO_BAND("_4", " 4", 4, 8, 632.0f, 1587.0f),
        MB_SC_MONO_BAND("_5", " 5", 5, 8, 1587.0f, 3984.0f),
        MB_SC_MONO_BAND("_6", " 6", 6, 8, 3984.0f, 10000.0f),
        MB_SC_MONO_BAND("_7", " 7", 7, 8, 10000.0f, 20000.0f),

        PORTS_END
    };

    static const port_t sc_mb_expander_stereo_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        PORTS_STEREO_SIDECHAIN,
        MB_COMMON(exp_sc_bands),
        MB_CHANNEL("", ""),
        MB_FFT_METERS("_l", " Left"),
        MB_CHANNEL_METERS("_l", " Left"),
        MB_FFT_METERS("_r", " Right"),
        MB_CHANNEL_METERS("_r", " Right"),

        MB_SPLIT("_1", " 1", 0.0f, 40.0f),
        MB_SPLIT("_2", " 2", 1.0f, 100.0f),
        MB_SPLIT("_3", " 3", 0.0f, 252.0f),
        MB_SPLIT("_4", " 4", 1.0f, 632.0f),
        MB_SPLIT("_5", " 5", 0.0f, 1587.0f),
        MB_SPLIT("_6", " 6", 1.0f, 3984.0f),
        MB_SPLIT("_7", " 7", 0.0f, 10000.0f),

        MB_SC_STEREO_BAND("_0", " 0", 0, 8, 10.0f, 40.0f),
        MB_SC_STEREO_BAND("_1", " 1", 1, 8, 40.0f, 100.0f),
        MB_SC_STEREO_BAND("_2", " 2", 2, 8, 100.0f, 252.0f),
        MB_SC_STEREO_BAND("_3", " 3", 3, 8, 252.0f, 632.0f),
        MB_SC_STEREO_BAND("_4", " 4", 4, 8, 632.0f, 1587.0f),
        MB_SC_STEREO_BAND("_5", " 5", 5, 8, 1587.0f, 3984.0f),
        MB_SC_STEREO_BAND("_6", " 6", 6, 8, 3984.0f, 10000.0f),
        MB_SC_STEREO_BAND("_7", " 7", 7, 8, 10000.0f, 20000.0f),

        PORTS_END
    };

    static const port_t sc_mb_expander_lr_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        PORTS_STEREO_SIDECHAIN,
        MB_COMMON(exp_sc_lr_bands),
        MB_CHANNEL("_l", " Left"),
        MB_CHANNEL("_r", " Right"),
        MB_FFT_METERS("_l", " Left"),
        MB_CHANNEL_METERS("_l", " Left"),
        MB_FFT_METERS("_r", " Right"),
        MB_CHANNEL_METERS("_r", " Right"),

        MB_SPLIT("_1l", " 1 Left", 0.0f, 40.0f),
        MB_SPLIT("_2l", " 2 Left", 1.0f, 100.0f),
        MB_SPLIT("_3l", " 3 Left", 0.0f, 252.0f),
        MB_SPLIT("_4l", " 4 Left", 1.0f, 632.0f),
        MB_SPLIT("_5l", " 5 Left", 0.0f, 1587.0f),
        MB_SPLIT("_6l", " 6 Left", 1.0f, 3984.0f),
        MB_SPLIT("_7l", " 7 Left", 0.0f, 10000.0f),

        MB_SPLIT("_1r", " 1 Right", 0.0f, 40.0f),
        MB_SPLIT("_2r", " 2 Right", 1.0f, 100.0f),
        MB_SPLIT("_3r", " 3 Right", 0.0f, 252.0f),
        MB_SPLIT("_4r", " 4 Right", 1.0f, 632.0f),
        MB_SPLIT("_5r", " 5 Right", 0.0f, 1587.0f),
        MB_SPLIT("_6r", " 6 Right", 1.0f, 3984.0f),
        MB_SPLIT("_7r", " 7 Right", 0.0f, 10000.0f),

        MB_SC_STEREO_BAND("_0l", " 0 Left", 0, 8, 10.0f, 40.0f),
        MB_SC_STEREO_BAND("_1l", " 1 Left", 1, 8, 40.0f, 100.0f),
        MB_SC_STEREO_BAND("_2l", " 2 Left", 2, 8, 100.0f, 252.0f),
        MB_SC_STEREO_BAND("_3l", " 3 Left", 3, 8, 252.0f, 632.0f),
        MB_SC_STEREO_BAND("_4l", " 4 Left", 4, 8, 632.0f, 1587.0f),
        MB_SC_STEREO_BAND("_5l", " 5 Left", 5, 8, 1587.0f, 3984.0f),
        MB_SC_STEREO_BAND("_6l", " 6 Left", 6, 8, 3984.0f, 10000.0f),
        MB_SC_STEREO_BAND("_7l", " 7 Left", 7, 8, 10000.0f, 20000.0f),

        MB_SC_STEREO_BAND("_0r", " 0 Right", 0, 8, 10.0f, 40.0f),
        MB_SC_STEREO_BAND("_1r", " 1 Right", 1, 8, 40.0f, 100.0f),
        MB_SC_STEREO_BAND("_2r", " 2 Right", 2, 8, 100.0f, 252.0f),
        MB_SC_STEREO_BAND("_3r", " 3 Right", 3, 8, 252.0f, 632.0f),
        MB_SC_STEREO_BAND("_4r", " 4 Right", 4, 8, 632.0f, 1587.0f),
        MB_SC_STEREO_BAND("_5r", " 5 Right", 5, 8, 1587.0f, 3984.0f),
        MB_SC_STEREO_BAND("_6r", " 6 Right", 6, 8, 3984.0f, 10000.0f),
        MB_SC_STEREO_BAND("_7r", " 7 Right", 7, 8, 10000.0f, 20000.0f),

        PORTS_END
    };

    static const port_t sc_mb_expander_ms_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        PORTS_STEREO_SIDECHAIN,
        MB_COMMON(exp_sc_ms_bands),
        MB_CHANNEL("_m", " Mid"),
        MB_CHANNEL("_s", " Side"),
        MB_FFT_METERS("_m", " Mid"),
        MB_CHANNEL_METERS("_l", " Left"),
        MB_FFT_METERS("_s", " Side"),
        MB_CHANNEL_METERS("_r", " Right"),

        MB_SPLIT("_1m", " 1 Mid", 0.0f, 40.0f),
        MB_SPLIT("_2m", " 2 Mid", 1.0f, 100.0f),
        MB_SPLIT("_3m", " 3 Mid", 0.0f, 252.0f),
        MB_SPLIT("_4m", " 4 Mid", 1.0f, 632.0f),
        MB_SPLIT("_5m", " 5 Mid", 0.0f, 1587.0f),
        MB_SPLIT("_6m", " 6 Mid", 1.0f, 3984.0f),
        MB_SPLIT("_7m", " 7 Mid", 0.0f, 10000.0f),

        MB_SPLIT("_1s", " 1 Side", 0.0f, 40.0f),
        MB_SPLIT("_2s", " 2 Side", 1.0f, 100.0f),
        MB_SPLIT("_3s", " 3 Side", 0.0f, 252.0f),
        MB_SPLIT("_4s", " 4 Side", 1.0f, 632.0f),
        MB_SPLIT("_5s", " 5 Side", 0.0f, 1587.0f),
        MB_SPLIT("_6s", " 6 Side", 1.0f, 3984.0f),
        MB_SPLIT("_7s", " 7 Side", 0.0f, 10000.0f),

        MB_SC_STEREO_BAND("_0m", " 0 Mid", 0, 8, 10.0f, 40.0f),
        MB_SC_STEREO_BAND("_1m", " 1 Mid", 1, 8, 40.0f, 100.0f),
        MB_SC_STEREO_BAND("_2m", " 2 Mid", 2, 8, 100.0f, 252.0f),
        MB_SC_STEREO_BAND("_3m", " 3 Mid", 3, 8, 252.0f, 632.0f),
        MB_SC_STEREO_BAND("_4m", " 4 Mid", 4, 8, 632.0f, 1587.0f),
        MB_SC_STEREO_BAND("_5m", " 5 Mid", 5, 8, 1587.0f, 3984.0f),
        MB_SC_STEREO_BAND("_6m", " 6 Mid", 6, 8, 3984.0f, 10000.0f),
        MB_SC_STEREO_BAND("_7m", " 7 Mid", 7, 8, 10000.0f, 20000.0f),

        MB_SC_STEREO_BAND("_0s", " 0 Side", 0, 8, 10.0f, 40.0f),
        MB_SC_STEREO_BAND("_1s", " 1 Side", 1, 8, 40.0f, 100.0f),
        MB_SC_STEREO_BAND("_2s", " 2 Side", 2, 8, 100.0f, 252.0f),
        MB_SC_STEREO_BAND("_3s", " 3 Side", 3, 8, 252.0f, 632.0f),
        MB_SC_STEREO_BAND("_4s", " 4 Side", 4, 8, 632.0f, 1587.0f),
        MB_SC_STEREO_BAND("_5s", " 5 Side", 5, 8, 1587.0f, 3984.0f),
        MB_SC_STEREO_BAND("_6s", " 6 Side", 6, 8, 3984.0f, 10000.0f),
        MB_SC_STEREO_BAND("_7s", " 7 Side", 7, 8, 10000.0f, 20000.0f),

        PORTS_END
    };

    // Multiband expander
    const plugin_metadata_t  mb_expander_mono_metadata::metadata =
    {
        "Multi-band Expander Mono x8",
        "Multiband Expander Mono x8",
        "MBE8M",
        &developers::v_sadovnikov,
        "mb_expander_mono",
        "----",
        LSP_MB_EXPANDER_BASE + 0,
        LSP_VERSION(1, 0, 0),
        mb_expander_classes,
        E_INLINE_DISPLAY,
        mb_expander_mono_ports,
        "dynamics/expander/multiband/mono.xml",
        NULL,
        mono_plugin_port_groups
    };

    const plugin_metadata_t  mb_expander_stereo_metadata::metadata =
    {
        "Multi-band Expander Stereo x8",
        "Multiband Expander Stereo x8",
        "MBE8S",
        &developers::v_sadovnikov,
        "mb_expander_stereo",
        "----",
        LSP_MB_EXPANDER_BASE + 1,
        LSP_VERSION(1, 0, 0),
        mb_expander_classes,
        E_INLINE_DISPLAY,
        mb_expander_stereo_ports,
        "dynamics/expander/multiband/stereo.xml",
        NULL,
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  mb_expander_lr_metadata::metadata =
    {
        "Multi-band Expander LeftRight x8",
        "Multiband Expander LeftRight x8",
        "MBE8LR",
        &developers::v_sadovnikov,
        "mb_expander_lr",
        "----",
        LSP_MB_EXPANDER_BASE + 2,
        LSP_VERSION(1, 0, 0),
        mb_expander_classes,
        E_INLINE_DISPLAY,
        mb_expander_lr_ports,
        "dynamics/expander/multiband/lr.xml",
        NULL,
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  mb_expander_ms_metadata::metadata =
    {
        "Multi-band Expander MidSide x8",
        "Multiband Expander MidSide x8",
        "MBE8MS",
        &developers::v_sadovnikov,
        "mb_expander_ms",
        "----",
        LSP_MB_EXPANDER_BASE + 3,
        LSP_VERSION(1, 0, 0),
        mb_expander_classes,
        E_INLINE_DISPLAY,
        mb_expander_ms_ports,
        "dynamics/expander/multiband/ms.xml",
        NULL,
        stereo_plugin_port_groups
    };


    const plugin_metadata_t  sc_mb_expander_mono_metadata::metadata =
    {
        "Sidechain Multi-band Expander Mono x8",
        "Sidechain Multiband Expander Mono x8",
        "SCMBE8M",
        &developers::v_sadovnikov,
        "sc_mb_expander_mono",
        "----",
        LSP_MB_EXPANDER_BASE + 4,
        LSP_VERSION(1, 0, 0),
        mb_expander_classes,
        E_INLINE_DISPLAY,
        sc_mb_expander_mono_ports,
        "dynamics/expander/multiband/mono.xml",
        NULL,
        mono_plugin_sidechain_port_groups
    };

    const plugin_metadata_t  sc_mb_expander_stereo_metadata::metadata =
    {
        "Sidechain Multi-band Expander Stereo x8",
        "Sidechain Multiband Expander Stereo x8",
        "SCMBE8S",
        &developers::v_sadovnikov,
        "sc_mb_expander_stereo",
        "----",
        LSP_MB_EXPANDER_BASE + 5,
        LSP_VERSION(1, 0, 0),
        mb_expander_classes,
        E_INLINE_DISPLAY,
        sc_mb_expander_stereo_ports,
        "dynamics/expander/multiband/stereo.xml",
        NULL,
        stereo_plugin_sidechain_port_groups
    };

    const plugin_metadata_t  sc_mb_expander_lr_metadata::metadata =
    {
        "Sidechain Multi-band Expander LeftRight x8",
        "Sidechain Multiband Expander LeftRight x8",
        "SCMBE8LR",
        &developers::v_sadovnikov,
        "sc_mb_expander_lr",
        "----",
        LSP_MB_EXPANDER_BASE + 6,
        LSP_VERSION(1, 0, 0),
        mb_expander_classes,
        E_INLINE_DISPLAY,
        sc_mb_expander_lr_ports,
        "dynamics/expander/multiband/lr.xml",
        NULL,
        stereo_plugin_sidechain_port_groups
    };

    const plugin_metadata_t  sc_mb_expander_ms_metadata::metadata =
    {
        "Sidechain Multi-band Expander MidSide x8",
        "Sidechain Multiband Expander MidSide x8",
        "SCMBE8MS",
        &developers::v_sadovnikov,
        "sc_mb_expander_ms",
        "----",
        LSP_MB_EXPANDER_BASE + 7,
        LSP_VERSION(1, 0, 0),
        mb_expander_classes,
        E_INLINE_DISPLAY,
        sc_mb_expander_ms_ports,
        "dynamics/expander/multiband/ms.xml",
        NULL,
        stereo_plugin_sidechain_port_groups
    };
}

