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
        const char *RawArray::CLASS_NAME = "[";
        
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

            // Patch item type
            const char *prim = primitive_type_name(enItemType);
            if (prim == NULL)
            {
                if (!tmp.remove(0, 2))
                    return STATUS_BAD_TYPE;
                if (tmp.last() == ';')
                {
                    if (!tmp.remove_last())
                        return STATUS_BAD_TYPE;
                }
            }
            else if (!tmp.set_ascii(prim))
                return STATUS_NO_MEM;
            sItemType.swap(&tmp);

            // Allocate empty data
            pData       = ::calloc(items, size_of(enItemType));
            if (pData == NULL)
                return STATUS_NO_MEM;

            return STATUS_OK;
        }

        bool RawArray::instanceof(const char *name) const
        {
            if (name == CLASS_NAME)
                return true;
            if (!::strcmp(name, CLASS_NAME))
                return true;
            return Object::instanceof(name);
        }

        status_t RawArray::to_string_padded(LSPString *dst, size_t pad)
        {
            if (!dst->fmt_append_utf8("*%p = new %s[%d] ", this, sItemType.get_utf8(), nLength))
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
                    if (!pad_string(dst, pad))
                        return STATUS_NO_MEM;
                    Object *obj = *(ptr.p_object++);
                    bool res = (obj != NULL) ?
                        (obj->to_string_padded(dst, pad) == STATUS_OK) :
                        dst->append_ascii("null\n");
                    if (! res)
                        return STATUS_NO_MEM;
                }
                --pad;
                if (!pad_string(dst, pad))
                    return STATUS_NO_MEM;
                if (!dst->append_ascii("}\n"))
                    return STATUS_NO_MEM;
            }
            else
            {
                if (!dst->append_ascii("{ "))
                    return STATUS_NO_MEM;

                for (size_t i=0; i<nLength; ++i)
                {
                    bool res = (i > 0) ? dst->append_ascii(", ") : true;
                    if (! res)
                        return STATUS_NO_MEM;

                    switch (enItemType)
                    {
                        case JFT_BYTE:      res = dst->fmt_append_utf8("%d", *(ptr.p_byte++)); break;
                        case JFT_DOUBLE:    res = dst->fmt_append_utf8("%f", *(ptr.p_double++)); break;
                        case JFT_FLOAT:     res = dst->fmt_append_utf8("%f", *(ptr.p_float++)); break;
                        case JFT_INTEGER:   res = dst->fmt_append_utf8("%d", int(*(ptr.p_int++))); break;
                        case JFT_LONG:      res = dst->fmt_append_utf8("%lld", (long long)(*(ptr.p_long++))); break;
                        case JFT_SHORT:     res = dst->fmt_append_utf8("%d", int(*(ptr.p_short++))); break;
                        case JFT_BOOL:      res = dst->fmt_append_utf8("%s", (*(ptr.p_bool++)) ? "true" : "false"); break;
                        case JFT_CHAR:
                            res = dst->append('\'');
                            if (res)
                                res = dst->append(lsp_wchar_t(*(ptr.p_char++)));
                            if (res)
                                res = dst->append('\'');
                            break;
                        default:
                            return STATUS_CORRUPTED;
                    }
                    if (! res)
                        return STATUS_NO_MEM;
                }
                if (!dst->append_ascii(" }\n"))
                    return STATUS_NO_MEM;
            }

            return STATUS_OK;
        }
    
    } /* namespace java */
} /* namespace lsp */
