/*
 * metadata.cpp
 *
 *  Created on: 08 окт. 2015 г.
 *      Author: sadko
 */

#include <stddef.h>
#include <core/metadata.h>
#include <core/types.h>

#include <string.h>

namespace lsp
{
    const port_t lv2_atom_ports[] =
    {
        // Input audio ports
        { LSP_LV2_ATOM_PORT_IN,     "Communication IN",     U_NONE,         R_UI_SYNC, F_IN, 0, 0, 0, 0, NULL       },
        { LSP_LV2_ATOM_PORT_OUT,    "Communication OUT",    U_NONE,         R_UI_SYNC, F_OUT, 0, 0, 0, 0, NULL      },

        { NULL, NULL }
    };

    const port_t lv2_latency_port =
    {
        LSP_LV2_LATENCY_PORT,   "Latency OUT",          U_NONE,         R_CONTROL, F_OUT | F_INT | F_LOWER | F_UPPER, 0, MAX_SAMPLE_RATE, 0, 0, NULL
    };

    const char *unit_names[] =
    {
        NULL,
        NULL,
        "%",

        "mm",
        "cm",
        "m",
        "\"",
        "km",

        "samp",

        "Hz",
        "kHz",
        "MHz",
        "bpm",

        "cent",

        "bar",
        "beat",
        "s",
        "ms",

        "dB",
        "G",
        "G",

        "°C",
        "°F",
        "°K",

        NULL
    };

    const char *encode_unit(size_t unit)
    {
        if ((unit >= 0) && (unit <= U_ENUM))
            return unit_names[unit];

        return NULL;
    }

    unit_t decode_unit(const char *name)
    {
        for (ssize_t i=0; i<= U_ENUM; ++i)
        {
            const char *uname = unit_names[i];
            if ((uname != NULL) && (!strcmp(name, uname)))
                return unit_t(i);
        }
        return U_NONE;
    }

    bool is_discrete_unit(unit_t unit)
    {
        switch (unit)
        {
            case U_BOOL:
            case U_SAMPLES:
            case U_ENUM:
                return true;
            default:
                break;
        }
        return false;
    }

    bool is_decibel_unit(unit_t unit)
    {
        switch (unit)
        {
            case U_DB:
            case U_GAIN_AMP:
            case U_GAIN_POW:
                return true;
            default:
                break;
        }
        return false;
    }

    size_t list_size(const char **list)
    {
        size_t size = 0;
        while ((list != NULL) && (*list != NULL))
        {
            size    ++;
            list    ++;
        }
        return size;
    }
}


