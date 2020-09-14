/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Stefano Tronci <stefano.tronci@protonmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 2 May 2017
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
    static const int latency_meter_classes[] = { C_ANALYSER, -1};

    static const port_t latency_meter_ports[] =
    {
        PORTS_MONO_PLUGIN,
        BYPASS,
        CONTROL("mlat", "Max expected latency", U_MSEC, latency_meter_metadata::LATENCY),
        CONTROL("pthr", "Peak threshold", U_GAIN_AMP, latency_meter_metadata::PEAK_THRESHOLD),
        CONTROL("athr", "Absolute threshold", U_GAIN_AMP, latency_meter_metadata::ABS_THRESHOLD),
        AMP_GAIN10("gin", "Input Gain", 1.0f),
        SWITCH("fback", "Feedback", 0.0f),
        AMP_GAIN10("gout", "Output Gain", 1.0f),
        TRIGGER("ttrig", "Trig a Latency measurement"),
        METER("l_v", "Latency Value", U_MSEC, latency_meter_metadata::MTR_LATENCY),
        METER_GAIN20("ilvl", "Input Level"),

        PORTS_END
    };

    const plugin_metadata_t latency_meter_metadata::metadata =
    {
        "Latenzmessgerät",
        "Latency Meter",
        "LM1M",
        &developers::s_tronci,
        "latency_meter",
        "abee",
        LSP_LATENCY_METER_BASE + 0,
        LSP_VERSION(1, 0, 0),
        latency_meter_classes,
        E_NONE,
        latency_meter_ports,
        "util/latency_meter.xml",
        NULL,
        mono_plugin_port_groups
    };
}
