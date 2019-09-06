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
            pParent     = NULL;
            pRawName    = NULL;
            nSuid       = 0;
            nFlags      = 0;
            nFields     = 0;
            nSlots      = 0;
            nSizeOf     = 0;
            vFields     = NULL;
            vSlots      = NULL;
        }
        
        ObjectStreamClass::~ObjectStreamClass()
        {
            if (vFields != NULL)
            {
                for (size_t i=0; i<nFields; ++i)
                {
                    if (vFields[i] != NULL)
                    {
                        delete vFields[i];
                        vFields[i] = NULL;
                    }
                }
                ::free(vFields);
                vFields = NULL;
            }
            if (vSlots != NULL)
            {
                for (size_t i=0; i<nSlots; ++i)
                    if (vSlots[i] != NULL)
                        vSlots[i] = NULL;
                ::free(vSlots);
                vSlots = NULL;
            }
            if (pRawName != NULL)
            {
                ::free(pRawName);
                pRawName = NULL;
            }
        }
    
    } /* namespace java */
} /* namespace lsp */
