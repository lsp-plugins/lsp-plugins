/*
 * ObjectStreamField.cpp
 *
 *  Created on: 30 авг. 2019 г.
 *      Author: sadko
 */

#include <stdlib.h>
#include <core/files/java/Object.h>
#include <core/files/java/ObjectStreamField.h>

namespace lsp
{
    namespace java
    {
        const char *ObjectStreamField::CLASS_NAME    = "java.io.ObjectStreamField";
        
        ObjectStreamField::ObjectStreamField(): Object(CLASS_NAME)
        {
            enType      = JFT_UNKNOWN;
            pSignature  = NULL;
            nOffset     = 0;
            sRawName    = NULL;
        }
        
        ObjectStreamField::~ObjectStreamField()
        {
            pSignature  = NULL;
            if (sRawName != NULL)
                ::free(sRawName);
            sRawName    = NULL;
        }

    } /* namespace java */
} /* namespace lsp */
