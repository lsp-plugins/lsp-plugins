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

#include <core/files/java/Handles.h>
#include <stdlib.h>

#define CAP_GRANULARITY         0x400

namespace lsp
{
    namespace java
    {
        
        Handles::Handles()
        {
            nCapacity       = 0;
            vItems          = NULL;
            nHandle         = 0;
        }
        
        Handles::~Handles()
        {
            flush();
        }

        Object *Handles::get(size_t handle)
        {
            return (handle < nHandle) ? vItems[handle] : NULL;
        }

        status_t Handles::assign(Object *obj)
        {
            if (obj == NULL)
                return STATUS_BAD_ARGUMENTS;

            size_t cap = ((nHandle + CAP_GRANULARITY) / CAP_GRANULARITY) * CAP_GRANULARITY;
            if (nCapacity < cap)
            {
                Object **x = reinterpret_cast<Object **>(::realloc(vItems, sizeof(Object *) * cap));
                if (x == NULL)
                    return STATUS_NO_MEM;
                for (size_t i=nCapacity; i<cap; ++i)
                    x[i]        = NULL;
                vItems      = x;
                nCapacity   = cap;
            }

            vItems[nHandle++]   = obj;
            return STATUS_OK;
        }

        void Handles::clear()
        {
            if (vItems == NULL)
                return;
            for (size_t i=0; i<nCapacity; ++i)
            {
                if (vItems[i] == NULL)
                    continue;
                delete vItems[i];
                vItems[i] = NULL;
            }

            nHandle     = 0;
        }

        void Handles::flush()
        {
            if (vItems == NULL)
                return;
            for (size_t i=0; i<nCapacity; ++i)
            {
                if (vItems[i] == NULL)
                    continue;
                delete vItems[i];
            }
            ::free(vItems);
            vItems      = NULL;
            nCapacity   = 0;
            nHandle     = 0;
        }
    
    } /* namespace java */
} /* namespace lsp */
