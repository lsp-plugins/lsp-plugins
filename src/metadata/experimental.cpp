/*
 * experimental.cpp
 *
*  Created on: 08 апр. 2016 г.
 *      Author: sadko
 */

#include <metadata/plugins.h>
#include <metadata/ports.h>
#include <metadata/developers.h>

namespace lsp
{
#ifndef LSP_NO_EXPERIMENTAL
    //-------------------------------------------------------------------------
    // Impulse responses
    static const port_t impulse_responses_mono_ports[] =
    {
        // Input audio ports
        AUDIO_INPUT_MONO,

        // Output audio ports
        AUDIO_OUTPUT_MONO,

        // Input controls
        BYPASS,
        { "ifn",            "Impulse file",     U_STRING,       R_PATH,     F_IN, 0, 0, 0, 0, NULL },
        { "ifc",            "Channel",          U_ENUM,         R_CONTROL,  F_IN | F_INT, 0, 0, 0, 0, file_channels },
        { "ifl",            "Length",           U_PERCENT,      R_CONTROL,  F_IN | F_UPPER | F_LOWER | F_STEP,
            impulse_responses_base_metadata::CONVLEN_MIN, impulse_responses_base_metadata::CONVLEN_MAX, impulse_responses_base_metadata::CONVLEN_DFL, impulse_responses_base_metadata::CONVLEN_STEP, NULL },
        DRY_GAIN(0.0f),
        WET_GAIN(1.0f),

        OUT_GAIN,

        // Output controls

        PORTS_END
    };

    static const int impulse_responses_classes[] = { C_CONVERTER, C_SPECTRAL, -1 };

    const plugin_metadata_t  impulse_responses_metadata::metadata =
    {
        "Impulsantworten Mono",
        "Impulse Responses Mono",
        "IR1M",
        &developers::v_sadovnikov,
        "impulse_responses_mono",
        "xxxx",
        LSP_LADSPA_BASE + 1000,
        LSP_VERSION(1, 0, 0),
        impulse_responses_classes,
        impulse_responses_mono_ports,
        NULL
    };
#endif /* LSP_NO_EXPERMIENTAL */

}



