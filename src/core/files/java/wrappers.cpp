/*
 * wrappers.cpp
 *
 *  Created on: 6 сент. 2019 г.
 *      Author: sadko
 */

#include <core/files/java/wrappers.h>

namespace lsp
{
    namespace java
    {
        #define WRAPPER_DEF(name, type_t) \
            const char *name::CLASS_NAME = "java.lang." #name; \
            \
            name::name(): Object(CLASS_NAME) {} \
            \
            name::~name() {} \
            \
            type_t name::value() const \
            { \
                if (nSlots <= 0) return 0; \
                object_slot_t *s = &vSlots[nSlots-1]; \
                if (s->size < sizeof(type_t)) return 0; \
                type_t *ptr = reinterpret_cast<type_t *>(&vData[s->offset]); \
                return *ptr; \
            } \
            \
            status_t name::get_value(type_t *dst) const \
            { \
                if (nSlots <= 0) return STATUS_CORRUPTED; \
                object_slot_t *s = &vSlots[nSlots-1]; \
                if (s->size < sizeof(type_t)) return STATUS_CORRUPTED; \
                type_t *ptr = reinterpret_cast<type_t *>(&vData[s->offset]); \
                if (dst != NULL) *dst = *ptr; \
                return STATUS_OK; \
            } \

        WRAPPER_DEF(Byte, byte_t);
        WRAPPER_DEF(Short, short_t);
        WRAPPER_DEF(Integer, int_t);
        WRAPPER_DEF(Long, long_t);
        WRAPPER_DEF(Double, double_t);
        WRAPPER_DEF(Float, float_t);
        WRAPPER_DEF(Boolean, bool_t);
        WRAPPER_DEF(Character, char_t);

        status_t Byte::to_string_padded(LSPString *dst, size_t pad)
        {
            return (dst->fmt_append_ascii("*%p = new Byte(%d)\n", this, int(value()))) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Short::to_string_padded(LSPString *dst, size_t pad)
        {
            return (dst->fmt_append_ascii("*%p = new Short(%d)\n", this, int(value()))) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Integer::to_string_padded(LSPString *dst, size_t pad)
        {
            return (dst->fmt_append_ascii("*%p = new Integer(%d)\n", this, int(value()))) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Long::to_string_padded(LSPString *dst, size_t pad)
        {
            return (dst->fmt_append_ascii("*%p = new Long(%d)\n", this, int(value()))) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Double::to_string_padded(LSPString *dst, size_t pad)
        {
            return (dst->fmt_append_ascii("*%p = new Double(%f)\n", this, value())) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Float::to_string_padded(LSPString *dst, size_t pad)
        {
            return (dst->fmt_append_ascii("*%p = new Float(%f)\n", this, value())) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Boolean::to_string_padded(LSPString *dst, size_t pad)
        {
            return (dst->fmt_append_ascii("*%p = new Boolean(%s)\n", this, (value()) ? "true" : "false")) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Character::to_string_padded(LSPString *dst, size_t pad)
        {
            if (!dst->fmt_append_ascii("*%p = new Character('", this))
                return STATUS_NO_MEM;
            dst->append(lsp_wchar_t(value()));
            return (dst->append_ascii("')\n")) ? STATUS_OK : STATUS_NO_MEM;
        }

        #undef WRAPPER_DEF
    }
}

