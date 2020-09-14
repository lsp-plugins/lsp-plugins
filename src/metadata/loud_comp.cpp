/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 30 июн. 2020 г.
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
    static const int loud_comp_classes[] = { C_AMPLIFIER, -1 };

    static const port_item_t loud_comp_fft_rank[] =
    {
        { "256",    NULL },
        { "512",    NULL },
        { "1024",   NULL },
        { "2048",   NULL },
        { "4096",   NULL },
        { "8192",   NULL },
        { "16384",  NULL },
        { NULL, NULL }
    };

    static const port_item_t loud_comp_std[] =
    {
        { "Flat",               "lcomp.curve.flat" },
        { "ISO226-2003",        "lcomp.curve.iso226" },
        { "Fletcher-Munson",    "lcomp.curve.fm" },
        { "Robinson-Dadson",    "lcomp.curve.rd" },
        { NULL, NULL }
    };

    #define LOUD_COMP_COMMON \
        AMP_GAIN("input", "Input gain", GAIN_AMP_0_DB, GAIN_AMP_P_72_DB), \
        COMBO("std", "Loudness contour standard", loud_comp_base_metadata::STD_DFL, loud_comp_std), \
        COMBO("fft", "FFT size", loud_comp_base_metadata::FFT_RANK_IDX_DFL, loud_comp_fft_rank), \
        CONTROL("volume", "Output volume", U_DB, loud_comp_base_metadata::PHONS), \
        SWITCH("refer", "Enable reference sine generator", 0.0f), \
        SWITCH("hclip", "Hard-clipping enable", 0.0f), \
        CONTROL("hcrange", "Hard-clipping range", U_DB, loud_comp_base_metadata::HCRANGE), \
        TRIGGER("hcclean", "Clean hard-clipping indicators"), \
        MESH("spec", "Level compensation frequency graph", 2, loud_comp_base_metadata::CURVE_MESH_SIZE), \
        SWITCH("relspec", "Equalization curve is shown relative to the volume", 0.0f)

    static const port_t loud_comp_mono_ports[] =
    {
        PORTS_MONO_PLUGIN,
        BYPASS,
        LOUD_COMP_COMMON,
        METER_GAIN("ilm", "Input level meter", GAIN_AMP_P_24_DB),
        BLINK("olc", "Output level clip"),
        METER_GAIN("olm", "Output level meter", GAIN_AMP_P_24_DB),
        PORTS_END
    };

    static const port_t loud_comp_stereo_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        BYPASS,
        LOUD_COMP_COMMON,
        METER_GAIN("ilm_l", "Input level meter Left", GAIN_AMP_P_24_DB),
        METER_GAIN("ilm_r", "Input level meter Right", GAIN_AMP_P_24_DB),
        BLINK("olc_l", "Output level clip Left"),
        BLINK("olc_r", "Output level clip Right"),
        METER_GAIN("olm_l", "Output level meter Left", GAIN_AMP_P_24_DB),
        METER_GAIN("olm_r", "Output level meter Right", GAIN_AMP_P_24_DB),
        PORTS_END
    };

    // Multiband gate
    const plugin_metadata_t  loud_comp_mono_metadata::metadata =
    {
        "Lautstärke Kompensator Mono",
        "Loudness Compensator Mono",
        "LK1M",
        &developers::v_sadovnikov,
        "loud_comp_mono",
        "eno9",
        LSP_LOUD_COMP_BASE + 0,
        LSP_VERSION(1, 0, 0),
        loud_comp_classes,
        E_INLINE_DISPLAY | E_DUMP_STATE,
        loud_comp_mono_ports,
        "util/loud_comp.xml",
        NULL,
        mono_plugin_port_groups
    };

    const plugin_metadata_t  loud_comp_stereo_metadata::metadata =
    {
        "Lautstärke Kompensator Stereo",
        "Loudness Compensator Stereo",
        "LK1S",
        &developers::v_sadovnikov,
        "loud_comp_stereo",
        "wva0",
        LSP_LOUD_COMP_BASE + 1,
        LSP_VERSION(1, 0, 0),
        loud_comp_classes,
        E_INLINE_DISPLAY | E_DUMP_STATE,
        loud_comp_stereo_ports,
        "util/loud_comp.xml",
        NULL,
        stereo_plugin_port_groups
    };
}
