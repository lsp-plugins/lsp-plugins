/*
 * IDictionary.cpp
 *
 *  Created on: 26 февр. 2020 г.
 *      Author: sadko
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
