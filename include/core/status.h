/*
 * status.h
 *
 *  Created on: 25 марта 2016 г.
 *      Author: sadko
 */

#ifndef CORE_STATUS_H_
#define CORE_STATUS_H_

typedef int     status_t;

namespace lsp
{
    enum status_codes
    {
        STATUS_OK,
        STATUS_UNSPECIFIED,
        STATUS_LOADING,
        STATUS_IN_PROCESS,
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
        STATUS_UNSUPPORTED_DEVICE,
        STATUS_OPENED,
        STATUS_BAD_TYPE,
        STATUS_CORRUPTED,
        STATUS_INSUFFICIENT,
        STATUS_KILLED,
        STATUS_TIMED_OUT,
        STATUS_FAILED,
        STATUS_SKIP,
        STATUS_CANCELLED,
        STATUS_NOT_EMPTY,
        STATUS_IS_DIRECTORY,
        STATUS_NOT_DIRECTORY,
        STATUS_REMOVED,
        STATUS_BREAK_POINT, // This is special status for step-by-step tracing algorithms
        STATUS_READONLY,
        STATUS_NULL,
        STATUS_LOCKED,
        STATUS_REJECTED,
        STATUS_ALREADY_BOUND,
        STATUS_NO_CAPTURES,
        STATUS_NO_SOURCES,
        STATUS_BAD_PATH,
        STATUS_PROTOCOL_ERROR,
        STATUS_BAD_TOKEN,
        STATUS_NO_GRAB,
        STATUS_UNDERFLOW,

        STATUS_TOTAL,
        STATUS_MAX = STATUS_TOTAL - 1,
        STATUS_SUCCESS = STATUS_OK
    };

    const char *get_status(status_t code);
    const char *get_status_lc_key(status_t code);

    bool status_is_success(status_t code);
    bool status_is_preliminary(status_t code);
    bool status_is_error(status_t code);
}

#endif /* CORE_STATUS_H_ */
