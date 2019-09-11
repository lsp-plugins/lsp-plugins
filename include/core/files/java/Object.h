/*
 * Object.h
 *
 *  Created on: 29 авг. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_JAVA_OBJECT_H_
#define CORE_FILES_JAVA_OBJECT_H_

#include <common/types.h>
#include <core/LSPString.h>
#include <core/files/java/const.h>

namespace lsp
{
    namespace java
    {
        typedef uint32_t            handle_t;

        class ObjectStreamClass;
        class RawArray;
        class Enum;
        class String;

        typedef struct object_slot_t
        {
            ObjectStreamClass      *desc;   // Slot descriptor
            size_t                  offset; // Offset from data beginning
            size_t                  size;   // Size in bytes
            size_t                  __pad;
        } object_slot_t;

        /**
         * This is common Java object implementation,
         * should be always allocated with new() operator
         */
        class Object
        {
            public:
                static const char *CLASS_NAME;

            private:
                friend class ObjectStream;
                friend class RawArray;
                Object & operator = (const Object &);

            protected:
                const char     *pClass;
                object_slot_t  *vSlots;
                size_t          nSlots;
                uint8_t        *vData;

            private:
                template <class type_t, class cast_t>
                    inline status_t    read_reference(const char *field, type_t *item, ftype_t type) const;

                template <class type_t, class wrapper_t>
                    inline status_t    read_prim_item(const char *field, type_t *item, ftype_t type) const;

            protected:
                virtual status_t to_string_padded(LSPString *dst, size_t pad);
                static bool pad_string(LSPString *dst, size_t pad);

            public:
                explicit Object(const char *class_name);
                virtual ~Object();

            public:
                inline const char *class_name() const { return pClass; }

                virtual bool instanceof(const char *name) const;

            public:
                status_t    get_byte(const char *field, byte_t *dst) const;
                status_t    get_short(const char *field, short_t *dst) const;
                status_t    get_int(const char *field, int_t *dst) const;
                status_t    get_long(const char *field, long_t *dst) const;
                status_t    get_float(const char *field, float_t *dst) const;
                status_t    get_double(const char *field, double_t *dst) const;
                status_t    get_char(const char *field, char_t *dst) const;
                status_t    get_bool(const char *field, bool_t *dst) const;
                status_t    get_array(const char *field, const RawArray **dst) const;
                status_t    get_object(const char *field, const Object **dst) const;
                status_t    get_string(const char *field, const String **dst) const;
                status_t    get_string(const char *field, LSPString *dst) const;
                status_t    get_string(const char *field, const char **dst) const;
                status_t    get_enum(const char *field, const Enum **dst) const;
                status_t    get_enum(const char *field, LSPString *dst) const;
                status_t    get_enum(const char *field, const char **dst) const;

            public:
                template <class T>
                    inline T *cast() { return (instanceof(T::CLASS_NAME)) ? static_cast<T *>(this) : NULL; }

                template <class T>
                    inline const T *cast() const { return (instanceof(T::CLASS_NAME)) ? static_cast<const T *>(this) : NULL; }

                status_t to_string(LSPString *dst) const;
        };

    } /* namespace java */
} /* namespace lsp */

#endif /* CORE_FILES_JAVA_OBJECT_H_ */
