/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 26 февр. 2020 г.
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

#ifndef CORE_I18N_IDICTIONARY_H_
#define CORE_I18N_IDICTIONARY_H_

#include <core/status.h>
#include <core/io/Path.h>
#include <core/LSPString.h>

namespace lsp
{
    class IDictionary
    {
        private:
            IDictionary & operator = (const IDictionary &);

        public:
            explicit IDictionary();
            virtual ~IDictionary();

        public:
            /**
             * Initialize dictionary
             * @param path the location of the dictionary
             * @return status of operation
             */
            virtual status_t init(const char *path);

            /**
             * Initialize dictionary
             * @param path the location of the dictionary
             * @return status of operation
             */
            virtual status_t init(const LSPString *path);

            /**
             * Initialize dictionary
             * @param path the location of the dictionary
             * @return status of operation
             */
            virtual status_t init(const io::Path *path);

            /**
             * Lookup for a key
             * @param key non-null UTF-8 encoded key value
             * @param value pointer to store the value (may be NULL)
             * @return status of operation
             */
            virtual status_t lookup(const char *key, LSPString *value);

            /**
             * Lookup for a key
             * @param key non-null key value
             * @param value pointer to store the value (may be NULL)
             * @return status of operation
             */
            virtual status_t lookup(const LSPString *key, LSPString *value);

            /**
             * Lookup for a dictionary
             * @param key non-null UTF-8 encoded key value
             * @param value pointer to store the value (may be NULL)
             * @return status of operation
             */
            virtual status_t lookup(const char *key, IDictionary **value);

            /**
             * Lookup for a dictionary
             * @param key non-null key value
             * @param value pointer to store the value (may be NULL)
             * @return status of operation
             */
            virtual status_t lookup(const LSPString *key, IDictionary **value);

            /**
             * Get the element by index of element
             * @param index index of the element
             * @param key the key name
             * @param value pointer to store the value
             * @return status of operation
             */
            virtual status_t get_value(size_t index, LSPString *key, LSPString *value);

            /**
             * Get the child dictionary by index of element
             * @param index index of the element
             * @param key the key name
             * @param dict dictionary to return
             * @return pointer to child dictionary or NULL if there is no such child
             */
            virtual status_t get_child(size_t index, LSPString *key, IDictionary **dict);

            /**
             * Return size of dictionary in elements
             * @return size of dictionary in elements
             */
            virtual size_t size();
    };

} /* namespace lsp */

#endif /* CORE_I18N_IDICTIONARY_H_ */
