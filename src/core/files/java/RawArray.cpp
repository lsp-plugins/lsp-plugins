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

        status_t RawArray::to_string_padded(LSPString *dst, size_t pad)
        {
            if (!dst->fmt_append_utf8("%p = new %s[%d] ", this, sItemType.get_utf8(), nLength))
                return STATUS_NO_MEM;
            if (nLength <= 0)
                return (dst->append_ascii("{ }\n")) ? STATUS_OK : STATUS_NO_MEM;

            prim_ptr_t ptr;
            ptr.p_void      = pData;
            if ((enItemType == JFT_ARRAY) || (enItemType == JFT_OBJECT))
            {
                if (!dst->append_ascii("{\n"))
                    return STATUS_NO_MEM;
                ++pad;
                for (size_t i=0; i<nLength; ++i)
                {
                    Object *obj = *(ptr.p_object++);
                    bool res = (obj != NULL) ?
                        (obj->to_string_padded(dst, pad) == STATUS_OK) :
                        dst->append_ascii("null\n");
                    if (! res)
                        return STATUS_NO_MEM;
                }
                --pad;
            }
            else
            {
                if (!dst->append_ascii("{ "))
                    return STATUS_NO_MEM;

                for (size_t i=0; i<nLength; ++i)
                {
                    bool res = (i > 0) ? dst->append_ascii(", ") : true;
                    if (res != STATUS_OK)
                        return STATUS_NO_MEM;

                    switch (enItemType)
                    {
                        case JFT_BYTE:      res = dst->fmt_append_utf8("%d\n", *(ptr.p_byte++)); break;
                        case JFT_DOUBLE:    res = dst->fmt_append_utf8("%f\n", *(ptr.p_double++)); break;
                        case JFT_FLOAT:     res = dst->fmt_append_utf8("%f\n", *(ptr.p_float++)); break;
                        case JFT_INTEGER:   res = dst->fmt_append_utf8("%d\n", int(*(ptr.p_int++))); break;
                        case JFT_LONG:      res = dst->fmt_append_utf8("%lld\n", (long long)(*(ptr.p_long++))); break;
                        case JFT_SHORT:     res = dst->fmt_append_utf8("%d\n", int(*(ptr.p_short++))); break;
                        case JFT_BOOL:      res = dst->fmt_append_utf8("%s\n", (*(ptr.p_bool++)) ? "true" : "false"); break;
                        case JFT_CHAR:      res = dst->append(lsp_wchar_t(*(ptr.p_char++))); break;
                        default:
                            return STATUS_CORRUPTED;
                    }
                    if (res != STATUS_OK)
                        return STATUS_NO_MEM;
                }
            }
            if (!dst->append_ascii("}\n"))
                return STATUS_NO_MEM;

            return STATUS_OK;
        }
    
    } /* namespace java */
} /* namespace lsp */
