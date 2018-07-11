/*
 * status.cpp
 *
 *  Created on: 25 марта 2016 г.
 *      Author: sadko
 */

#include <core/status.h>

namespace lsp
{
    const char *status_descriptions[] =
    {
        "Success",
        "Unspecified",
        "Loading",
        "Unknown error",
        "Not enough memory",
        "Not found",
        "Bad format",
        "Unsupported format",
        "Corrupted file",
        "Invalid unique identifier",
        "Disconnected",
        NULL
    };

    const char *get_status(status_t code)
    {
        return ((code >= 0) && (code < STATUS_TOTAL)) ? status_descriptions[code] : NULL;
    }
}


