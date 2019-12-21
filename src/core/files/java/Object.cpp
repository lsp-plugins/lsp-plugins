/*
 * Object.cpp
 *
 *  Created on: 29 авг. 2019 г.
 *      Author: sadko
 */

#include <core/debug.h>
#include <core/files/java/defs.h>
#include <core/files/java/Object.h>
#include <core/files/java/String.h>
#include <core/files/java/RawArray.h>
#include <core/files/java/Enum.h>
#include <core/files/java/wrappers.h>
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

        bool Object::instanceof(const char *name) const
        {
            if (name == pClass)
                return true;
            return ::strcmp(name, pClass) == 0;
        }

        template <class type_t, class cast_t>
            inline status_t Object::read_reference(const char *field, type_t *item, ftype_t type) const
            {
                bool found = false;

                // Lookup slot
                for (ssize_t i=nSlots-1; i>=0; --i)
                {
                    const object_slot_t *s = &vSlots[i];
                    const ObjectStreamClass *os = s->desc;

                    // Lookup field
                    uint8_t *data       = &vData[s->offset];
                    for (size_t j=0, m=os->fields(); j<m; ++j)
                    {
                        // Align base of the current field
                        const ObjectStreamField *f = os->field(j);

                        // Check field match
                        if (::strcmp(f->raw_name(), field) == 0)
                        {
                            found = true;
                            if (f->is_reference())
                            {
                                // Null value?
                                const Object *obj = *reinterpret_cast<const Object **>(&data[f->offset()]);
                                if (obj == NULL)
                                    return STATUS_NULL;

                                // Can be cast?
                                const cast_t *w = obj->cast<cast_t>();
                                if (w != NULL)
                                {
                                    // Return the value
                                    if (item != NULL)
                                        *item = w;
                                    return STATUS_OK;
                                }
                            } // is_reference
                        } // strcmp
                    } // for
                }

                return (found) ? STATUS_BAD_TYPE : STATUS_NOT_FOUND;
            }

        template <class type_t, class wrapper_t>
            inline status_t Object::read_prim_item(const char *field, type_t *item, ftype_t type) const
            {
                bool found = false;

                // Lookup slot
                for (ssize_t i=nSlots-1; i>=0; --i)
                {
                    const object_slot_t *s = &vSlots[i];
                    const ObjectStreamClass *os = s->desc;

                    // Lookup field
                    uint8_t *data       = &vData[s->offset];
                    for (size_t j=0, m=os->fields(); j<m; ++j)
                    {
                        // Align base of the current field
                        const ObjectStreamField *f = os->field(j);

                        // Check field match
                        if (::strcmp(f->raw_name(), field) == 0)
                        {
                            found = true;

                            // Type match?
                            if (f->type() == type)
                            {
                                if (item != NULL)
                                    *item           = *reinterpret_cast<const type_t *>(&data[f->offset()]);
                                return STATUS_OK;
                            }
                            else if (f->is_reference()) // Reference type?
                            {
                                // Need to check for wrappers (e.g. java.lang.Long -> long)
                                // Null value?
                                const Object *obj = *reinterpret_cast<const Object **>(&data[f->offset()]);
                                if (obj == NULL)
                                    return STATUS_NULL;

                                // Can be cast to wrapper?
                                const wrapper_t *w = obj->cast<const wrapper_t>();
                                if (w != NULL)
                                    return w->get_value(item);
                            }
                        }
                    }
                }

                return (found) ? STATUS_BAD_TYPE : STATUS_NOT_FOUND;
            }

        status_t Object::get_byte(const char *field, byte_t *dst) const
        {
            return read_prim_item<byte_t, Byte>(field, dst, JFT_BYTE);
        }

        status_t Object::get_short(const char *field, short_t *dst) const
        {
            return read_prim_item<short_t, Short>(field, dst, JFT_SHORT);
        }

        status_t Object::get_int(const char *field, int_t *dst) const
        {
            return read_prim_item<int_t, Integer>(field, dst, JFT_INTEGER);
        }

        status_t Object::get_long(const char *field, long_t *dst) const
        {
            return read_prim_item<long_t, Long>(field, dst, JFT_LONG);
        }

        status_t Object::get_float(const char *field, float_t *dst) const
        {
            return read_prim_item<float_t, Float>(field, dst, JFT_FLOAT);
        }

        status_t Object::get_double(const char *field, double_t *dst) const
        {
            return read_prim_item<double_t, Double>(field, dst, JFT_DOUBLE);
        }

        status_t Object::get_char(const char *field, char_t *dst) const
        {
            return read_prim_item<char_t, Character>(field, dst, JFT_CHAR);
        }

        status_t Object::get_bool(const char *field, bool_t *dst) const
        {
            return read_prim_item<bool_t, Boolean>(field, dst, JFT_BOOL);
        }

        status_t Object::get_object(const char *field, const Object **dst) const
        {
            return read_reference<const Object *, Object>(field, dst, JFT_OBJECT);
        }

        status_t Object::get_array(const char *field, const RawArray **dst) const
        {
            return read_reference<const RawArray *, RawArray>(field, dst, JFT_ARRAY);
        }

        status_t Object::get_enum(const char *field, const Enum **dst) const
        {
            const Enum *en = NULL;
            status_t res = read_reference<const Enum *, Enum>(field, &en, JFT_OBJECT);
            if (res != STATUS_OK)
                return res;
            if (dst != NULL)
                *dst    = en;
            return STATUS_OK;
        }

        status_t Object::get_enum(const char *field, LSPString *dst) const
        {
            const Enum *en = NULL;
            status_t res = get_enum(field, &en);
            if (res != STATUS_OK)
                return res;
            else if (en == NULL)
                return STATUS_NULL;
            if (dst == NULL)
                return STATUS_OK;

            return (dst->set(en->name())) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Object::get_enum(const char *field, const char **dst) const
        {
            const Enum *en = NULL;
            status_t res = get_enum(field, &en);
            if (res != STATUS_OK)
                return res;
            else if (en == NULL)
                return STATUS_NULL;
            if (dst != NULL)
                *dst = en->name()->get_utf8();

            return STATUS_OK;
        }

        status_t Object::get_string(const char *field, const String **dst) const
        {
            const String *str = NULL;
            status_t res = read_reference<const String *, String>(field, &str, JFT_OBJECT);
            if (res != STATUS_OK)
                return res;
            if (dst != NULL)
                *dst    = str;
            return STATUS_OK;
        }

        status_t Object::get_string(const char *field, LSPString *dst) const
        {
            const String *str = NULL;
            status_t res = get_string(field, &str);
            if (res != STATUS_OK)
                return res;
            else if (str == NULL)
                return STATUS_NULL;
            if (dst == NULL)
                return STATUS_OK;

            return (dst->set(str->string())) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Object::get_string(const char *field, const char **dst) const
        {
            const String *str = NULL;
            status_t res = get_string(field, &str);
            if (res != STATUS_OK)
                return res;
            else if (str == NULL)
                return STATUS_NULL;
            if (dst != NULL)
                *dst = str->string()->get_utf8();

            return STATUS_OK;
        }

        status_t Object::to_string_padded(LSPString *dst, size_t pad)
        {
//            lsp_trace("*%p = new ", this);
            if (!dst->fmt_append_ascii("*%p = new ", this))
                return STATUS_NO_MEM;
            if (!dst->append_utf8(pClass))
                return STATUS_NO_MEM;
            if (!dst->append_ascii(" {\n"))
                return STATUS_NO_MEM;

            ++pad;
            for (size_t i=0; i<nSlots; ++i)
            {
                prim_ptr_t ptr;

                object_slot_t *s        = &vSlots[i];
                ObjectStreamClass *os   = s->desc;
                uint8_t *data           = &vData[s->offset];

//                lsp_trace("i=%d, nslots=%d, s=%p, os=%p", int(i), int(nSlots), s, os);

                if (!pad_string(dst, pad))
                    return STATUS_NO_MEM;
//                lsp_trace("%s:\n", os->raw_name());
                if (!dst->fmt_append_utf8("%s:\n", os->raw_name()))
                    return STATUS_NO_MEM;

                ++pad;

                // Dump fields
                for (size_t j=0, n=os->fields(); j<n; ++j)
                {
                    const ObjectStreamField *f = os->field(j);
                    ptr.p_ubyte     = &data[f->offset()];

                    if (!pad_string(dst, pad))
                        return STATUS_NO_MEM;
//                    lsp_trace("ptr = %p (type=%d)", ptr.p_ubyte, int(f->type()));
                    if (!dst->fmt_append_utf8("%s = ", f->name()->get_utf8()))
                        return STATUS_NO_MEM;

                    bool res = true;
                    switch (f->type())
                    {
                        case JFT_BYTE:      res = dst->fmt_append_utf8("(byte) %d\n", *ptr.p_byte); break;
                        case JFT_DOUBLE:    res = dst->fmt_append_utf8("(double) %f\n", *ptr.p_double); break;
                        case JFT_FLOAT:     res = dst->fmt_append_utf8("(float) %f\n", *ptr.p_float); break;
                        case JFT_INTEGER:   res = dst->fmt_append_utf8("(int) %d\n", int(*ptr.p_int)); break;
                        case JFT_LONG:      res = dst->fmt_append_utf8("(long) %lld\n", (long long)(*ptr.p_long)); break;
                        case JFT_SHORT:     res = dst->fmt_append_utf8("(short) %d\n", int(*ptr.p_short)); break;
                        case JFT_BOOL:      res = dst->fmt_append_utf8("(bool) %s\n", (*ptr.p_bool) ? "true" : "false"); break;
                        case JFT_CHAR:
                            res = dst->append_ascii("'");
                            if (res)
                                res = dst->append(lsp_wchar_t(*ptr.p_char));
                            if (res)
                                res = dst->append_ascii("'\n");
                            break;
                        case JFT_ARRAY:
                        case JFT_OBJECT:
                        {
                            Object *obj = *ptr.p_object;
                            if (obj != NULL)
                                res = obj->to_string_padded(dst, pad) == STATUS_OK;
                            else
                                res = dst->append_ascii("null\n");
                            break;
                        }
                        default:
                            return STATUS_CORRUPTED;
                    }

                    if (!res)
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

            if (!pad_string(dst, pad))
                return STATUS_NO_MEM;
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

        status_t Object::to_string(LSPString *dst) const
        {
            Object *_this = const_cast<Object *>(this);
            return _this->to_string_padded(dst, 0);
        }

    } /* namespace java */
} /* namespace lsp */
