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
            return ::strcmp(name, CLASS_NAME) == 0;
        }
    
    } /* namespace java */
} /* namespace lsp */
