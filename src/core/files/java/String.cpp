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
#include <core/files/java/String.h>

namespace lsp
{
    namespace java
    {
        const char *String::CLASS_NAME  = "java.lang.String";

        String::String(): Object(CLASS_NAME)
        {
        }
        
        String::~String()
        {
        }

        status_t String::to_string_padded(LSPString *dst, size_t pad)
        {
            if (!dst->fmt_append_ascii("*%p = \"", this))
                return STATUS_NO_MEM;
            if (!dst->append(&sString))
                return STATUS_NO_MEM;
            return (dst->append_ascii("\"\n")) ? STATUS_OK : STATUS_NO_MEM;
        }
    
    } /* namespace java */
} /* namespace lsp */
