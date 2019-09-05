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

        bool Enum::instanceof(const char *name)
        {
            if (name == CLASS_NAME)
                return true;
            if (::strcmp(name, CLASS_NAME) == 0)
                return true;
            return Object::instanceof(name);
        }

        status_t Enum::to_string(LSPString *dst)
        {
            if (!dst->fmt_append_utf8("%p = enum %s ", this, class_name()))
                return STATUS_NO_MEM;
            return (dst->append(&sName)) ? STATUS_OK : STATUS_NO_MEM;
        }
    
    } /* namespace java */
} /* namespace lsp */
