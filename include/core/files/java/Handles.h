/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 29 авг. 2019 г.
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

#ifndef CORE_FILES_JAVA_HANDLES_H_
#define CORE_FILES_JAVA_HANDLES_H_

#include <common/types.h>
#include <core/files/java/Object.h>
#include <core/status.h>

namespace lsp
{
    namespace java
    {


        /**
         * This is implementation of storage for handles read from
         * the object stream
         */
        class Handles
        {
            protected:
                size_t          nCapacity;
                Object        **vItems;
                size_t          nHandle;

            public:
                explicit Handles();
                ~Handles();

            public:
                /**
                 * Allocate new handle identifier
                 * @return handle identifier
                 */
                inline size_t       new_handle() { return nHandle++; }

                /**
                 * Return number of handles
                 * @return number of handles
                 */
                inline size_t       handles() const { return nHandle; }

                /**
                 * Resolve the java object
                 * @param handle java object handle
                 * @return resolved java object or NULL if not exist
                 */
                Object *get(size_t handle);

                /**
                 * Bind object to the specified handle
                 * @param handle handle
                 * @param obj java object
                 * @return status of operation
                 */
                status_t assign(Object *obj);

                /**
                 * Clear handle registry and free handle table
                 */
                void    clear();

                /**
                 * Flush handle registry and free handle table
                 */
                void    flush();
        };
    
    } /* namespace java */
} /* namespace lsp */

#endif /* CORE_FILES_JAVA_OBJECTRESOLVER_H_ */
