/*
 * mb_expander.cpp
 *
 *  Created on: 27 дек. 2019 г.
 *      Author: sadko
 */

#include <core/types.h>

#include <metadata/plugins.h>
#include <metadata/ports.h>
#include <metadata/developers.h>

namespace lsp
{
    //-------------------------------------------------------------------------
    // Multiband expander
    static const int mb_expander_classes[] = { C_EXPANDER, -1 };

    static const port_t mb_expander_mono_ports[] =
    {
        PORTS_MONO_PLUGIN,

        PORTS_END
    };

    static const port_t mb_expander_stereo_ports[] =
    {
        PORTS_STEREO_PLUGIN,

        PORTS_END
    };

    static const port_t mb_expander_lr_ports[] =
    {
        PORTS_STEREO_PLUGIN,

        PORTS_END
    };

    static const port_t mb_expander_ms_ports[] =
    {
        PORTS_STEREO_PLUGIN,

        PORTS_END
    };

    static const port_t sc_mb_expander_mono_ports[] =
    {
        PORTS_MONO_PLUGIN,

        PORTS_END
    };

    static const port_t sc_mb_expander_stereo_ports[] =
    {
        PORTS_STEREO_PLUGIN,

        PORTS_END
    };

    static const port_t sc_mb_expander_lr_ports[] =
    {
        PORTS_STEREO_PLUGIN,

        PORTS_END
    };

    static const port_t sc_mb_expander_ms_ports[] =
    {
        PORTS_STEREO_PLUGIN,

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

