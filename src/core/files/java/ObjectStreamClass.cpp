/*
 * ClassDescriptor.cpp
 *
 *  Created on: 29 авг. 2019 г.
 *      Author: sadko
 */

#include <core/files/java/defs.h>
#include <core/files/java/ObjectStreamClass.h>

namespace lsp
{
    namespace java
    {
        const char *ObjectStreamClass::CLASS_NAME  = "java.io.ObjectStreamClass";
        
        ObjectStreamClass::ObjectStreamClass(): Object(CLASS_NAME)
        {
            nSuid       = 0;
            nFlags      = 0;
            nFields     = 0;
            vFields     = NULL;
        }
        
        ObjectStreamClass::~ObjectStreamClass()
        {
            if (vFields != NULL)
            {
                for (size_t i=0; i<nFields; ++i)
                    if (vFields[i] != NULL)
                    {
                        vFields[i]->release();
                        vFields[i] = NULL;
                    }
                ::free(vFields);
                vFields = NULL;
            }
        }
    
    } /* namespace java */
} /* namespace lsp */
