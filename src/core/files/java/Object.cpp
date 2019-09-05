/*
 * Object.cpp
 *
 *  Created on: 29 авг. 2019 г.
 *      Author: sadko
 */

#include <core/files/java/Object.h>
#include <core/files/java/ObjectStreamClass.h>

namespace lsp
{
    namespace java
    {
        const char *Object::CLASS_NAME       = "java.lang.Object";
        
        Object::Object(const char *class_name)
        {
            pClass      = class_name;
            vSlots      = NULL;
            nSlots      = 0;
            vData       = NULL;
        }
        
        Object::~Object()
        {
            if (vSlots != NULL)
                ::free(vSlots);
            if (vData != NULL)
                ::free(vData);

            pClass      = NULL;
        }

        bool Object::instanceof(const char *name)
        {
            if (name == pClass)
                return true;
            return ::strcmp(name, pClass) == 0;
        }

    } /* namespace java */
} /* namespace lsp */
