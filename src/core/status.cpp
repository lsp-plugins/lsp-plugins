/*
 * status.cpp
 *
 *  Created on: 25 марта 2016 г.
 *      Author: sadko
 */

#include <core/status.h>
#include <core/types.h>

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
        "No data",
        "Invalid unique identifier",
        "Disconnected",
        "Bad arguments",
        "Not bound",
        "Not implemented",
        "Already exists",
        "Overflow",
        "Bad hierarchy",
        "Duplicated",
        "Too big",
        "Permission denied",
        "I/O error",
        "End of file",
        "The resource is closed",
        "The feature is not supported",
        NULL
    };

    const char *get_status(status_t code)
    {
        return ((code >= 0) && (code < STATUS_TOTAL)) ? status_descriptions[code] : NULL;
    }
}


