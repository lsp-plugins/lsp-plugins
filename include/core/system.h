/*
 * system.h
 *
 *  Created on: 17 мар. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_SYSTEM_H_
#define CORE_SYSTEM_H_

#include <core/status.h>
#include <core/LSPString.h>
#include <core/io/Path.h>

namespace lsp
{
    namespace system
    {
        /**
         * Get environment variable
         * @param name environment variable name
         * @param dst string to store environment variable value, NULL for check-only
         * @return status of operation or STATUS_NOT_FOUND if there is no environment variable
         */
        status_t get_env_var(const LSPString *name, LSPString *dst);

        /**
         * Get environment variable
         * @param name environment variable name in UTF-8
         * @param dst string to store environment variable value, NULL for check-only
         * @return status of operation or STATUS_NOT_FOUND if there is no environment variable
         */
        status_t get_env_var(const char *name, LSPString *dst);

        /**
         * Set environment variable
         * @param name environment variable name
         * @param value environment variable value, NULL value deletes the variable
         * @return status of operation or STATUS_NOT_FOUND if there is no environment variable
         */
        status_t set_env_var(const LSPString *name, const LSPString *value);

        /**
         * Set environment variable
         * @param name environment variable name in UTF-8
         * @param value environment variable value in UTF-8, NULL value deletes the variable
         * @return status of operation or STATUS_NOT_FOUND if there is no environment variable
         */
        status_t set_env_var(const char *name, const char *value);

        /**
         * Set environment variable
         * @param name environment variable name in UTF-8
         * @param value environment variable value, NULL value deletes the variable
         * @return status of operation or STATUS_NOT_FOUND if there is no environment variable
         */
        status_t set_env_var(const char *name, const LSPString *value);

        /**
         * Remove environment variable
         * @param name variable to remove in UTF-8
         * @return status of operation
         */
        status_t remove_env_var(const char *name);

        /**
         * Remove environment variable
         * @param name variable to remove
         * @return status of operation
         */
        status_t remove_env_var(const LSPString *name);

        /**
         * Get current user's home directory
         * @param homedir pointer to string to store home directory path
         * @return status of operation
         */
        status_t get_home_directory(LSPString *homedir);

        /**
         * Get current user's home directory
         * @param homedir pointer to string to store home directory path
         * @return status of operation
         */
        status_t get_home_directory(io::Path *homedir);
    }
}


#endif /* CORE_SYSTEM_H_ */
