/*
 * trigger.cpp
 *
 *  Created on: 04 мая 2016 г.
 *      Author: sadko
 */

#include <metadata/plugins.h>
#include <metadata/ports.h>
#include <metadata/developers.h>

namespace lsp
{
#ifndef LSP_NO_EXPERIMENTAL
    //-------------------------------------------------------------------------
    // Trigger
    static const port_t trigger_mono_ports[] =
    {
        PORTS_MONO_PLUGIN,

        PORTS_END
    };

    static const port_t trigger_stereo_ports[] =
    {
        PORTS_STEREO_PLUGIN,

        PORTS_END
    };

    static const port_t trigger_mono_midi_ports[] =
    {
        PORTS_MONO_PLUGIN,
        PORTS_MIDI_CHANNEL,

        PORTS_END
    };

    static const port_t trigger_stereo_midi_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        PORTS_MIDI_CHANNEL,

        PORTS_END
    };

    static const int trigger_midi_classes[] = { C_CONVERTER, C_UTILITY, -1 };

    //-------------------------------------------------------------------------
    // Define plugin metadata
    const plugin_metadata_t  trigger_mono_metadata::metadata =
    {
        "Triggersensor Mono",
        "Trigger Mono",
        "TS1M",
        &developers::v_sadovnikov,
        "trigger_mono",
        "xxxx",
        0,
        LSP_VERSION(1, 0, 0),
        trigger_midi_classes,
        trigger_mono_midi_ports,
        NULL
    };

    const plugin_metadata_t  trigger_stereo_metadata::metadata =
    {
        "Triggersensor Stereo",
        "Trigger Stereo",
        "TS1S",
        &developers::v_sadovnikov,
        "trigger_stereo",
        "xxxx",
        0,
        LSP_VERSION(1, 0, 0),
        trigger_midi_classes,
        trigger_mono_midi_ports,
        NULL
    };

    const plugin_metadata_t  trigger_mono_midi_metadata::metadata =
    {
        "Triggersensor MIDI Mono",
        "Trigger MIDI Mono",
        "TSM1M",
        &developers::v_sadovnikov,
        "trigger_mono_midi",
        "xxxx",
        0,
        LSP_VERSION(1, 0, 0),
        trigger_midi_classes,
        trigger_mono_midi_ports,
        NULL
    };

    const plugin_metadata_t  trigger_stereo_midi_metadata::metadata =
    {
        "Triggersensor MIDI Stereo",
        "Trigger MIDI Stereo",
        "TSM1S",
        &developers::v_sadovnikov,
        "trigger_stereo_midi",
        "xxxx",
        0,
        LSP_VERSION(1, 0, 0),
        trigger_midi_classes,
        trigger_mono_midi_ports,
        NULL
    };
#endif
}
