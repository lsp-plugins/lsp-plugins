/*
 * metadata.cpp
 *
 *  Created on: 16 окт. 2015 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <core/windows.h>
#include <core/status.h>

#include <metadata/plugins.h>
#include <metadata/ports.h>

namespace lsp
{
    //-------------------------------------------------------------------------
    // Common port name definitions
    const char PORT_NAME_INPUT[]          = "in";
    const char PORT_NAME_OUTPUT[]         = "out";
    const char PORT_NAME_SIDECHAIN[]      = "sc";

    const char PORT_NAME_INPUT_L[]        = "in_l";
    const char PORT_NAME_INPUT_R[]        = "in_r";
    const char PORT_NAME_OUTPUT_L[]       = "out_l";
    const char PORT_NAME_OUTPUT_R[]       = "out_r";
    const char PORT_NAME_SIDECHAIN_L[]    = "sc_l";
    const char PORT_NAME_SIDECHAIN_R[]    = "sc_r";

    // Port groups
    const port_group_item_t mono_in_group_ports[] =
    {
        { PORT_NAME_INPUT,          PGR_CENTER      },
        { NULL }
    };

    const port_group_item_t mono_sidechain_group_ports[] =
    {
        { PORT_NAME_SIDECHAIN,      PGR_CENTER      },
        { NULL }
    };

    const port_group_item_t mono_out_group_ports[] =
    {
        { PORT_NAME_OUTPUT,         PGR_CENTER      },
        { NULL }
    };

    const port_group_item_t stereo_in_group_ports[] =
    {
        { PORT_NAME_INPUT_L,        PGR_LEFT        },
        { PORT_NAME_INPUT_R,        PGR_RIGHT       },
        { NULL }
    };

    const port_group_item_t stereo_sidechain_group_portss[] =
    {
        { PORT_NAME_SIDECHAIN_L,    PGR_LEFT        },
        { PORT_NAME_SIDECHAIN_R,    PGR_RIGHT       },
        { NULL }
    };

    const port_group_item_t stereo_out_group_ports[] =
    {
        { PORT_NAME_OUTPUT_L,       PGR_LEFT        },
        { PORT_NAME_OUTPUT_R,       PGR_RIGHT       },
        { NULL }
    };

    const port_group_t mono_plugin_port_groups[] =
    {
        { "mono_in",        "Mono Input",       GRP_MONO,       PGF_IN,     mono_in_group_ports         },
        { "mono_out",       "Mono Output",      GRP_MONO,       PGF_OUT,    mono_out_group_ports        },
        { NULL, NULL }
    };

    const port_group_t mono_plugin_sidechain_port_groups[] =
    {
        { "mono_in",        "Mono Input",       GRP_MONO,       PGF_IN,     mono_in_group_ports         },
        { "mono_out",       "Mono Output",      GRP_MONO,       PGF_OUT,    mono_out_group_ports        },
        { "sidechain_in",   "Sidechain Input",  GRP_MONO,       PGF_IN | PGF_SIDECHAIN,     mono_sidechain_group_ports, "mono_in"  },
        { NULL, NULL }
    };

    const port_group_t stereo_plugin_port_groups[] =
    {
        { "stereo_in",      "Stereo Input",     GRP_STEREO,     PGF_IN,     stereo_in_group_ports       },
        { "stereo_out",     "Stereo Output",    GRP_STEREO,     PGF_OUT,    stereo_out_group_ports      },
        { NULL, NULL }
    };

    const port_group_t stereo_plugin_sidechain_port_groups[] =
    {
        { "stereo_in",      "Stereo Input",     GRP_STEREO,     PGF_IN,     stereo_in_group_ports       },
        { "stereo_out",     "Stereo Output",    GRP_STEREO,     PGF_OUT,    stereo_out_group_ports      },
        { "sidechain_in",   "Sidechain Input",  GRP_STEREO,     PGF_IN | PGF_SIDECHAIN,     stereo_sidechain_group_portss, "stereo_in" },
        { NULL, NULL }
    };

    //-------------------------------------------------------------------------
    // Miscellaneous lists
    const char *file_channels[] =
    {
        "1",
        "2",
        "3",
        "4",
        "5",
        "6",
        "7",
        "8",
        NULL
    };

    const char *midi_channels[] =
    {
        "01", "02", "03", "04",
        "05", "06", "07", "08",
        "09", "10", "11", "12",
        "13", "14", "15", "16",
        NULL
    };

    const char *octaves[] =
    {
        "-2",
        "-1",
        "0",
        "1",
        "2",
        "3",
        "4",
        "5",
        "6",
        "7",
        "8",
        NULL
    };

    const char *notes[] =
    {
        "C",
        "C" UTF8_SHARP,
        "D",
        "D" UTF8_SHARP,
        "E",
        "F",
        "F" UTF8_SHARP,
        "G",
        "G" UTF8_SHARP,
        "A",
        "A" UTF8_SHARP,
        "B",
        NULL
    };
}



