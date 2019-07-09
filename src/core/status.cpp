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
        "In process",
        "Unknown error",
        "Not enough memory",
        "Not found or does not exist",
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
        "Unsupported device",
        "Opened",
        "Bad type",
        "Corrupted data",
        "Insufficient data",
        "Killed",
        "Timeout",
        "Failed",
        "Skip",
        "Execution has been cancelled",
        "Not empty",
        "Is a directory",
        "Not a directory",
        "Removed",
        "Debugging breakpoint",
        "Read-only device or file system",
        "The value is NULL",
        "The object is currently locked",
        "The operation has been rejected",
        "Already bound",
        "No valid captures",
        "No valid sources",

        NULL
    };

    const char *get_status(status_t code)
    {
        return ((code >= 0) && (code < STATUS_TOTAL)) ? status_descriptions[code] : NULL;
    }

    bool status_is_success(status_t code)
    {
        return code == STATUS_OK;
    }

    bool status_is_preliminary(status_t code)
    {
        switch (code)
        {
            case STATUS_IN_PROCESS:
            case STATUS_LOADING:
                return true;
        }
        return false;
    }

    bool status_is_error(status_t code)
    {
        if (status_is_success(code))
            return true;

        return ! status_is_preliminary(code);
    }
}


