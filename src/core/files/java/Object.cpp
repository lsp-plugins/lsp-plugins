/*
 * Object.cpp
 *
 *  Created on: 29 авг. 2019 г.
 *      Author: sadko
 */

#include <core/files/java/Object.h>

namespace lsp
{
    namespace java
    {
        const char *Object::CLASS_NAME       = "java.lang.Object";
        
        Object::Object(const char *class_name)
        {
            nReferences = 1;
            pClass      = class_name;
        }
        
        Object::~Object()
        {
            nReferences = 0;
            pClass      = NULL;
        }

        bool Object::instanceof(const char *name)
        {
            return ::strcmp(name, pClass) == 0;
        }

        ssize_t Object::acquire()
        {
            return ++nReferences;
        }

        ssize_t Object::release()
        {
            ssize_t refs = --nReferences;
            if (refs <= 0)
                delete this;
            return refs;
        }
    
    } /* namespace java */
} /* namespace lsp */
