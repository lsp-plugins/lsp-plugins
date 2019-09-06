/*
 * Enum.cpp
 *
 *  Created on: 5 сент. 2019 г.
 *      Author: sadko
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
