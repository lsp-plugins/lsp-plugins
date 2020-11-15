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

#include <core/files/java/defs.h>
#include <core/files/java/ObjectStreamClass.h>

namespace lsp
{
    namespace java
    {
        const char *ObjectStreamClass::CLASS_NAME  = "java.io.ObjectStreamClass";
        
        ObjectStreamClass::ObjectStreamClass(): Object(CLASS_NAME)
        {
            pParent     = NULL;
            pRawName    = NULL;
            nSuid       = 0;
            nFlags      = 0;
            nFields     = 0;
            nSlots      = 0;
            nSizeOf     = 0;
            vFields     = NULL;
            vSlots      = NULL;
        }
        
        ObjectStreamClass::~ObjectStreamClass()
        {
            if (vFields != NULL)
            {
                for (size_t i=0; i<nFields; ++i)
                {
                    if (vFields[i] != NULL)
                    {
                        delete vFields[i];
                        vFields[i] = NULL;
                    }
                }
                ::free(vFields);
                vFields = NULL;
            }
            if (vSlots != NULL)
            {
                for (size_t i=0; i<nSlots; ++i)
                    if (vSlots[i] != NULL)
                        vSlots[i] = NULL;
                ::free(vSlots);
                vSlots = NULL;
            }
            if (pRawName != NULL)
            {
                ::free(pRawName);
                pRawName = NULL;
            }
        }
    
    } /* namespace java */
} /* namespace lsp */
