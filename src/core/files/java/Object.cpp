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
        
        Object::Object(handle_t handle, const char *class_name)
        {
            nReferences = 1;
            nHandle     = handle;
            pClass      = class_name;
        }
        
        Object::~Object()
        {
            nReferences = 0;
            nHandle     = -1;
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
