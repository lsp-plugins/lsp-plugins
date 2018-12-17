/*
 * status.h
 *
 *  Created on: 25 марта 2016 г.
 *      Author: sadko
 */

#ifndef CORE_STATUS_H_
#define CORE_STATUS_H_

namespace lsp
{
    enum status_codes
    {
        STATUS_OK,
        STATUS_UNSPECIFIED,
        STATUS_LOADING,
        STATUS_UNKNOWN_ERR,
        STATUS_NO_MEM,
        STATUS_NOT_FOUND,
        STATUS_BAD_FORMAT,
        STATUS_UNSUPPORTED_FORMAT,
        STATUS_CORRUPTED_FILE,
        STATUS_NO_DATA,
        STATUS_INVALID_UID,
        STATUS_DISCONNECTED,
        STATUS_BAD_ARGUMENTS,
        STATUS_NOT_BOUND,
        STATUS_BAD_STATE,
        STATUS_NOT_IMPLEMENTED,
        STATUS_ALREADY_EXISTS,
        STATUS_OVERFLOW,
        STATUS_BAD_HIERARCHY,
        STATUS_DUPLICATED,
        STATUS_TOO_BIG,
        STATUS_PERMISSION_DENIED,
        STATUS_IO_ERROR,
        STATUS_NO_FILE,
        STATUS_EOF,
        STATUS_CLOSED,
        STATUS_NOT_SUPPORTED,
        STATUS_INVALID_VALUE,
        STATUS_BAD_LOCALE,
        STATUS_NO_DEVICE,
        STATUS_OPENED,
        STATUS_BAD_TYPE,

        STATUS_TOTAL,
        STATUS_MAX = STATUS_TOTAL - 1,
        STATUS_SUCCESS = STATUS_OK
    };

    typedef int     status_t;

    extern const char *status_descriptions[];

    const char *get_status(status_t code);
}

#endif /* CORE_STATUS_H_ */
