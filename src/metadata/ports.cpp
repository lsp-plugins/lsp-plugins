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
    const char PORT_NAME_BYPASS[]           = "bypass";

    const char PORT_NAME_INPUT[]            = "in";
    const char PORT_NAME_OUTPUT[]           = "out";
    const char PORT_NAME_SIDECHAIN[]        = "sc";

    const char PORT_NAME_INPUT_L[]          = "in_l";
    const char PORT_NAME_INPUT_R[]          = "in_r";
    const char PORT_NAME_OUTPUT_L[]         = "out_l";
    const char PORT_NAME_OUTPUT_R[]         = "out_r";
    const char PORT_NAME_SIDECHAIN_L[]      = "sc_l";
    const char PORT_NAME_SIDECHAIN_R[]      = "sc_r";

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

    const port_group_t mono_to_stereo_plugin_port_groups[] =
    {
        { "mono_in",        "Mono Input",       GRP_MONO,       PGF_IN,     mono_in_group_ports         },
        { "stereo_out",     "Stereo Output",    GRP_STEREO,     PGF_OUT,    stereo_out_group_ports      },
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
    const port_item_t file_channels[] =
    {
        { "1", NULL },
        { "2", NULL },
        { "3", NULL },
        { "4", NULL },
        { "5", NULL },
        { "6", NULL },
        { "7", NULL },
        { "8", NULL },
        { NULL, NULL }
    };

    const port_item_t midi_channels[] =
    {
        { "01", NULL },
        { "02", NULL },
        { "03", NULL },
        { "04", NULL },
        { "05", NULL },
        { "06", NULL },
        { "07", NULL },
        { "08", NULL },
        { "09", NULL },
        { "10", NULL },
        { "11", NULL },
        { "12", NULL },
        { "13", NULL },
        { "14", NULL },
        { "15", NULL },
        { "16", NULL },
        { NULL, NULL }
    };

    const port_item_t octaves[] =
    {
        { "-2", NULL },
        { "-1", NULL },
        { "0", NULL },
        { "1", NULL },
        { "2", NULL },
        { "3", NULL },
        { "4", NULL },
        { "5", NULL },
        { "6", NULL },
        { "7", NULL },
        { "8", NULL },
        { NULL, NULL }
    };

    const port_item_t notes[] =
    {
        { "C", NULL },
        { "C" UTF8_SHARP, NULL },
        { "D", NULL },
        { "D" UTF8_SHARP, NULL },
        { "E", NULL },
        { "F", NULL },
        { "F" UTF8_SHARP, NULL },
        { "G", NULL },
        { "G" UTF8_SHARP, NULL },
        { "A", NULL },
        { "A" UTF8_SHARP, NULL },
        { "B", NULL },
        { NULL, NULL }
    };

    const port_item_t fft_windows[] =
    {
        { "Hann",                   "fft.wnd.hann" },
        { "Hamming",                "fft.wnd.hamming" },
        { "Blackman",               "fft.wnd.blackman" },
        { "Lanczos",                "fft.wnd.lanczos" },
        { "Gaussian",               "fft.wnd.gauss" },
        { "Poisson",                "fft.wnd.poisson" },
        { "Parzen",                 "fft.wnd.parzen" },
        { "Tukey",                  "fft.wnd.tukey" },
        { "Welch",                  "fft.wnd.welch" },
        { "Nuttall",                "fft.wnd.nuttall" },
        { "Blackman-Nuttall",       "fft.wnd.blackman_nuttall" },
        { "Blackman-Harris",        "fft.wnd.blackman_harris" },
        { "Hann-Poisson",           "fft.wnd.hann_poisson" },
        { "Bartlett-Hann",          "fft.wnd.bartlett_hann" },
        { "Bartlett-Fejer",         "fft.wnd.bartlett_fejer" },
        { "Triangular",             "fft.wnd.triangular" },
        { "Rectangular",            "fft.wnd.rectangular" },
        { "Flat top",               "fft.wnd.flat_top" },
        { "Cosine",                 "fft.wnd.cosine" },
        { NULL, NULL }
    };

    const port_item_t fft_envelopes[] =
    {
        { "Violet noise",           "fft.env.violet" },
        { "Blue noise",             "fft.env.blue" },
        { "White noise",            "fft.env.white" },
        { "Pink noise",             "fft.env.pink" },
        { "Brown noise",            "fft.env.brown" },
        { "4.5 dB/oct fall-off",    "fft.env.falloff_4_5db" },
        { "4.5 dB/oct raise",       "fft.env.raise_4_5db" },
        { NULL, NULL }
    };
}



