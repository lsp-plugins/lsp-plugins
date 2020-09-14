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

#include <core/i18n/IDictionary.h>

namespace lsp
{
    
    IDictionary::IDictionary()
    {
    }
    
    IDictionary::~IDictionary()
    {
    }

    status_t IDictionary::init(const char *path)
    {
        if (path == NULL)
            return STATUS_BAD_ARGUMENTS;
        LSPString xpath;
        if (!xpath.set_utf8(path))
            return STATUS_NO_MEM;
        return init(&xpath);
    }

    status_t IDictionary::init(const io::Path *path)
    {
        if (path == NULL)
            return STATUS_BAD_ARGUMENTS;
        return init(path->as_string());
    }

    status_t IDictionary::init(const LSPString *path)
    {
        return STATUS_OK;
    }

    status_t IDictionary::lookup(const char *key, LSPString *value)
    {
        LSPString path;
        if (!path.set_utf8(key))
            return STATUS_NO_MEM;
        return lookup(&path, value);
    }

    status_t IDictionary::lookup(const LSPString *key, LSPString *value)
    {
        return STATUS_NOT_FOUND;
    }

    status_t IDictionary::lookup(const char *key, IDictionary **value)
    {
        LSPString path;
        if (!path.set_utf8(key))
            return STATUS_NO_MEM;
        return lookup(&path, value);
    }

    status_t IDictionary::lookup(const LSPString *key, IDictionary **value)
    {
        return STATUS_NOT_FOUND;
    }

    status_t IDictionary::get_value(size_t index, LSPString *key, LSPString *value)
    {
        return STATUS_NOT_FOUND;
    }

    status_t IDictionary::get_child(size_t index, LSPString *key, IDictionary **dict)
    {
        return STATUS_NOT_FOUND;
    }

    size_t IDictionary::size()
    {
        return 0;
    }

} /* namespace lsp */
