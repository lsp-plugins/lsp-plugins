/*
 * Object.cpp
 *
 *  Created on: 29 авг. 2019 г.
 *      Author: sadko
 */

#include <core/files/java/defs.h>
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

        status_t Object::to_string_padded(LSPString *dst, size_t pad)
        {
            if (!dst->fmt_append_ascii("%p = new class ", this))
                return STATUS_NO_MEM;
            if (!dst->append_utf8(pClass))
                return STATUS_NO_MEM;
            if (!dst->append_ascii(" {\n"))
                return STATUS_NO_MEM;

            ++pad;
            for (size_t i=0; i<nSlots; ++i)
            {
                prim_ptr_t ptr;

                object_slot_t *s = &vSlots[i];
                ObjectStreamClass *os = s->desc;

                if (!pad_string(dst, pad))
                    return STATUS_NO_MEM;
                if (!dst->fmt_append_utf8("%s:\n", os->class_name()))
                    return STATUS_NO_MEM;

                ++pad;
                // Dump fields
                ptr.p_ubyte     = &vData[s->offset];
                for (size_t j=0, n=os->fields(); j<n; ++j)
                {
                    if (!pad_string(dst, pad))
                        return STATUS_NO_MEM;
                    const ObjectStreamField *f = os->field(j);
                    if (!dst->fmt_append_utf8("%s = "))
                        return STATUS_NO_MEM;


                    bool res = true;
                    switch (f->type())
                    {
                        case JFT_BYTE:      res = dst->fmt_append_utf8("(byte) %d\n", *(ptr.p_byte++)); break;
                        case JFT_DOUBLE:    res = dst->fmt_append_utf8("(double) %f\n", *(ptr.p_double++)); break;
                        case JFT_FLOAT:     res = dst->fmt_append_utf8("(float) %f\n", *(ptr.p_float++)); break;
                        case JFT_INTEGER:   res = dst->fmt_append_utf8("(int) %d\n", int(*(ptr.p_int++))); break;
                        case JFT_LONG:      res = dst->fmt_append_utf8("(long) %lld\n", (long long)(*(ptr.p_long++))); break;
                        case JFT_SHORT:     res = dst->fmt_append_utf8("(short) %d\n", int(*(ptr.p_short++))); break;
                        case JFT_BOOL:      res = dst->fmt_append_utf8("(bool) %s\n", (*(ptr.p_bool++)) ? "true" : "false"); break;
                        case JFT_CHAR:
                            res = dst->append_ascii("(char) ");
                            if (res)
                                res = dst->append(lsp_wchar_t(*(ptr.p_char++)));
                            if (res)
                                res = dst->append('\n');
                            break;
                        case JFT_ARRAY:
                        case JFT_OBJECT:
                        {
                            Object *obj = *(ptr.p_object++);
                            if (obj != NULL)
                                res = obj->to_string_padded(dst, pad) == STATUS_OK;
                            else
                                res = dst->append_ascii("null\n");
                            break;
                        }
                        default:
                            return STATUS_CORRUPTED;
                    }

                    if (res != STATUS_OK)
                        return STATUS_NO_MEM;
                }
                // Dump data
                if (os->has_write_method())
                {
                    size_t rows     = (s->size + 0xf) >> 4;
                    uint8_t *curr   = &vData[s->offset];
                    uint8_t *end    = &vData[s->offset + s->size];

                    bool res        = true;
                    for (size_t j=0; j<rows; ++j)
                    {
                        if (!dst->fmt_append_ascii("%08x: ", int(j << 4)))
                            return STATUS_NO_MEM;

                        // Hex codes
                        for (size_t k=0; k<0x10; ++k)
                        {
                            if (&curr[k] < end)
                                res = dst->fmt_append_ascii("%02x ", int(curr[k]));
                            else
                                res = dst->append_ascii("   ");
                            if (res != STATUS_OK)
                                return STATUS_NO_MEM;
                        }

                        // Characters
                        for (size_t k=0; k<0x10; ++k)
                        {
                            if (&curr[k] < end)
                            {
                                char c      = curr[k];
                                if ((c < 0x20) || (c >= 0x80))
                                    c           = '.';
                                res = dst->append(c);
                            }
                            else
                                res = dst->append(' ');
                            if (res != STATUS_OK)
                                return STATUS_NO_MEM;
                        }
                        if (!dst->append('\n'))
                            return STATUS_NO_MEM;
                    }
                }
                --pad;
            }
            --pad;

            if (!dst->append_ascii("}\n"))
                return STATUS_NO_MEM;

            return STATUS_OK;
        }

        bool Object::pad_string(LSPString *dst, size_t pad)
        {
            pad *= 2;
            while (pad--)
                if (!dst->append(' '))
                    return false;
            return true;
        }

        status_t Object::to_string(LSPString *dst)
        {
            return to_string_padded(dst, 0);
        }

    } /* namespace java */
} /* namespace lsp */
