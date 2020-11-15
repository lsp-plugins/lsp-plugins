/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 5 сент. 2019 г.
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

#include <core/files/java/Enum.h>

namespace lsp
{
    namespace java
    {
        const char *Enum::CLASS_NAME = "java.lang.Enum";
        
        Enum::Enum(): Object(CLASS_NAME)
        {
        }
        
        Enum::~Enum()
        {
        }

        bool Enum::instanceof(const char *name) const
        {
            if (name == CLASS_NAME)
                return true;
            if (::strcmp(name, CLASS_NAME) == 0)
                return true;
            return Object::instanceof(name);
        }

        status_t Enum::to_string_padded(LSPString *dst, size_t pad)
        {
            if (!dst->fmt_append_utf8("*%p = %s.", this, class_name()))
                return STATUS_NO_MEM;
            if (!dst->append(&sName))
                return STATUS_NO_MEM;
            return (dst->append('\n')) ? STATUS_OK : STATUS_NO_MEM;
        }
    
    } /* namespace java */
} /* namespace lsp */
