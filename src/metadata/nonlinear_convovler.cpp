/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Stefano Tronci <stefano.tronci@protonmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 12 Apr 2018
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

#include <metadata/plugins.h>
#include <metadata/ports.h>
#include <metadata/developers.h>

namespace lsp
{
    static const int nonlinear_convolver_classes[] = { C_SIMULATOR, -1};

    static const port_item_t nc_order[] =
    {
        { "2", NULL },
        { "3", NULL },
        { "4", NULL },
        { "6", NULL },
        { "8", NULL },
        { NULL, NULL }
    };

    static const port_item_t nc_windowSize[] =
    {
        { "512", NULL },
        { "1024", NULL },
        { "2048", NULL },
        { "4096", NULL },
        { "8192", NULL },
        { "16384", NULL },
        { "32768", NULL },
        { "65536", NULL },
        { NULL, NULL }
    };

    static const port_t nonlinear_convolver_ports[] =
    {
        PORTS_MONO_PLUGIN,
        BYPASS,
        PATH("ifpn", "Input file"),
        STATUS("ifps", "Load status"),
        AMP_GAIN10("outg", "Output Gain", nonlinear_convolver_mono_metadata::GAIN_DFL),
        COMBO("ords", "Order", nonlinear_convolver_mono_metadata::MODEL_ORDER_DFL, nc_order),
        COMBO("wszs", "Window Size", nonlinear_convolver_mono_metadata::WSIZE_ORDER_DFL, nc_windowSize),
        TRIGGER("prep", "Trigger DSP Preparation"),
        MESH("krnl", "Kernels", 2, nonlinear_convolver_mono_metadata::KERNELS_MESH_SIZE),

        PORTS_END
    };

    const plugin_metadata_t nonlinear_convolver_mono_metadata::metadata =
    {
        "Nonlinear Convolver Mono",
        "Nonlinear Convolver Mono",
        "NC1M", // Nonlinear Convolver x1 Mono
        &developers::s_tronci,
        "nonlinear_convolver_mono",
        "sntq",
        LSP_NONLINEAR_CONVOLVER_BASE + 0,
        LSP_VERSION(1, 0, 0),
        nonlinear_convolver_classes,
        E_NONE,
        nonlinear_convolver_ports,
        "convolution/nonlinear_convolver.xml",
        NULL,
        mono_plugin_port_groups
    };
}
