/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 17 мар. 2019 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef CORE_SYSTEM_H_
#define CORE_SYSTEM_H_

#include <common/types.h>
#include <core/status.h>
#include <core/LSPString.h>
#include <core/io/Path.h>

namespace lsp
{
    namespace system
    {
        /**
         * Time information
         */
        typedef struct time_t
        {
            size_t  seconds;    /* The value in seconds */
            size_t  nanos;      /* The value in nanoseconds between 0 and 10^9-1 */
        } time_t;

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

        /**
         * Get user's local configuration path
         * @param path string to store user's configuration path
         * @return status of operation
         */
        status_t get_user_config_path(LSPString *path);

        /**
         * Get user's local configuration path
         * @param path string to store user's configuration path
         * @return status of operation
         */
        status_t get_user_config_path(io::Path *path);

        /**
         * Get current high-precision time
         * @param time pointer to structure to store time value
         */
        void get_time(time_t *time);

        /**
         * Get temporary directory
         * @param path pointer to store result
         * @return status of operation
         */
        status_t get_temporary_dir(LSPString *path);

        /**
         * Get temporary directory
         * @param path pointer to store result
         * @return status of operation
         */
        status_t get_temporary_dir(io::Path *path);
    }
}


#endif /* CORE_SYSTEM_H_ */
