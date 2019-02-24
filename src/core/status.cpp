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
        "Bad state",
        "Not implemented",
        "Already exists",
        "Overflow",
        "Bad hierarchy",
        "Duplicated",
        "Too big",
        "Permission denied",
        "I/O error",
        "No file",
        "End of file",
        "The resource is closed",
        "The feature is not supported",
        "Invalid value",
        "Bad locale",
        "No device",
        "Opened",
        "Bad type",
        "Insufficient data",
        "Killed",
        "Timeout",
        "Failed",
        "Skip",
        "Cancelled execution",
        "Debugging breakpoint",
        NULL
    };

    const char *get_status(status_t code)
    {
        return ((code >= 0) && (code < STATUS_TOTAL)) ? status_descriptions[code] : NULL;
    }
}


