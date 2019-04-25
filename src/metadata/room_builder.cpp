/*
 * room_builder.cpp
 *
 *  Created on: 25 апр. 2019 г.
 *      Author: sadko
 */

#include <metadata/plugins.h>
#include <metadata/ports.h>
#include <metadata/developers.h>

namespace lsp
{
    static const port_t room_builder_mono_ports[] =
    {
        // Input audio ports
        AUDIO_INPUT_MONO,
        AUDIO_OUTPUT_LEFT,
        AUDIO_OUTPUT_RIGHT,

        PORTS_END
    };

    static const port_t room_builder_stereo_ports[] =
    {
        // Input audio ports
        PORTS_STEREO_PLUGIN,

        PORTS_END
    };

    static const int room_builder_classes[] = { C_REVERB, -1 };

    const plugin_metadata_t  room_builder_mono_metadata::metadata =
    {
        "Raumbauer Mono",
        "Room Builder Mono",
        "RB1M",
        &developers::v_sadovnikov,
        "room_builder_mono",
        "????",
        0,
        LSP_VERSION(1, 0, 0),
        room_builder_classes,
        E_NONE,
        room_builder_mono_ports,
        "convolution/room_builder/mono.xml",
        mono_to_stereo_plugin_port_groups
    };

    const plugin_metadata_t  room_builder_stereo_metadata::metadata =
    {
        "Raumbauer Stereo",
        "Room Builder Stereo",
        "RB1S",
        &developers::v_sadovnikov,
        "room_builder_stereo",
        "????",
        0,
        LSP_VERSION(1, 0, 0),
        room_builder_classes,
        E_NONE,
        room_builder_stereo_ports,
        "convolution/room_builder/stereo.xml",
        stereo_plugin_port_groups
    };
}


