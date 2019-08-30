/*
 * ClassDescriptor.cpp
 *
 *  Created on: 29 авг. 2019 г.
 *      Author: sadko
 */

#include <core/files/java/defs.h>
#include <core/files/java/ClassDescriptor.h>

namespace lsp
{
    namespace java
    {
        
        ClassDescriptor::ClassDescriptor(size_t handle): Object(handle, CLASSNAME_OBJECTSTREAMCLASS)
        {
        }
        
        ClassDescriptor::~ClassDescriptor()
        {
            // TODO Auto-generated destructor stub
        }
    
    } /* namespace java */
} /* namespace lsp */
