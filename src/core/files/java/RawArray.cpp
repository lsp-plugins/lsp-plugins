/*
 * RawArray.cpp
 *
 *  Created on: 30 авг. 2019 г.
 *      Author: sadko
 */

#include <core/files/java/defs.h>
#include <core/files/java/RawArray.h>

namespace lsp
{
    namespace java
    {
        
        RawArray::RawArray(const char *xc): Object(xc)
        {
            nLength     = 0;
            enItemType  = JFT_UNKNOWN;
            pData       = NULL;
        }
        
        RawArray::~RawArray()
        {
            if (pData != NULL)
            {
                ::free(pData);
                pData       = NULL;
            }

            nLength     = 0;
        }

        status_t RawArray::allocate(size_t items)
        {
            LSPString tmp;
            if (!tmp.set_utf8(class_name()))
                return STATUS_NO_MEM;
            if (tmp.length() < 2)
                return STATUS_BAD_TYPE;
            if (tmp.first() != PTC_ARRAY)
                return STATUS_BAD_TYPE;

            // Decode item type
            enItemType  = decode_primitive_type(tmp.char_at(1));
            if (enItemType == JFT_UNKNOWN)
                return STATUS_BAD_TYPE;

            // Get item size
            nLength     = items;
            sItemType.swap(&tmp);

            // Allocate empty data
            pData       = ::calloc(items, size_of(enItemType));
            if (pData == NULL)
                return STATUS_NO_MEM;

            return STATUS_OK;
        }
    
    } /* namespace java */
} /* namespace lsp */
