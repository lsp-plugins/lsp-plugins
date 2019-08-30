/*
 * String.cpp
 *
 *  Created on: 29 авг. 2019 г.
 *      Author: sadko
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
    
    } /* namespace java */
} /* namespace lsp */
