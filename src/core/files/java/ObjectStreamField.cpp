/*
 * ObjectStreamField.cpp
 *
 *  Created on: 30 авг. 2019 г.
 *      Author: sadko
 */

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
        }
        
        ObjectStreamField::~ObjectStreamField()
        {
            if (pSignature != NULL)
            {
                pSignature->release();
                pSignature   = NULL;
            }
        }

        size_t ObjectStreamField::size_of() const
        {
            switch (enType)
            {
                case JFT_BYTE: return sizeof(uint8_t);
                case JFT_CHAR: return sizeof(lsp_utf16_t);
                case JFT_DOUBLE: return sizeof(double);
                case JFT_FLOAT: return sizeof(float);
                case JFT_INTEGER: return sizeof(uint32_t);
                case JFT_LONG: return sizeof(uint64_t);
                case JFT_SHORT: return sizeof(uint16_t);
                case JFT_BOOL: return sizeof(uint8_t);
                case JFT_ARRAY:
                case JFT_OBJECT:
                    return 0;
            }
            return -1;
        }
    
    } /* namespace java */
} /* namespace lsp */
