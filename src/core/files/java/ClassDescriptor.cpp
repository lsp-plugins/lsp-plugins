/*
 * ClassDescriptor.cpp
 *
 *  Created on: 29 авг. 2019 г.
 *      Author: sadko
 */

#include <core/files/java/ClassDescriptor.h>

namespace lsp
{
    namespace java
    {
        
        ClassDescriptor::ClassDescriptor(size_t handle)
        {
            nHandle     = handle;
        }
        
        ClassDescriptor::~ClassDescriptor()
        {
            // TODO Auto-generated destructor stub
        }
    
    } /* namespace java */
} /* namespace lsp */
