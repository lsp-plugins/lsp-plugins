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
        String::String(handle_t handle): Object(handle, CLASSNAME_STRING)
        {
        }
        
        String::~String()
        {
        }
    
    } /* namespace java */
} /* namespace lsp */
