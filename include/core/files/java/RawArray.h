/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 30 авг. 2019 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef CORE_FILES_JAVA_RAWARRAY_H_
#define CORE_FILES_JAVA_RAWARRAY_H_

#include <core/LSPString.h>
#include <core/files/java/Object.h>
#include <core/files/java/ObjectStreamField.h>

namespace lsp
{
    namespace java
    {
        class RawArray: public Object
        {
            public:
                static const char *CLASS_NAME;

            private:
                friend class ObjectStream;
                RawArray & operator = (const RawArray &);

            private:
                size_t      nLength;
                ftype_t     enItemType;
                LSPString   sItemType;
                void       *pData;

            private:
                status_t    allocate(size_t items);
                template <class type_t>
                    inline      type_t *get()   { return reinterpret_cast<type_t *>(pData); }

            protected:
                virtual status_t to_string_padded(LSPString *dst, size_t pad);

            public:
                explicit RawArray(const char *xc);
                virtual ~RawArray();

            public:
                inline size_t           length() const          { return nLength; }
                inline ftype_t          item_type() const       { return enItemType; }
                inline size_t           item_size_of() const    { return size_of(enItemType); }
                inline const LSPString *item_type_name() const  { return &sItemType; }

                virtual bool instanceof(const char *name) const;

            public:
                #define __GET_DEF(otype, name, jft_type) \
                    inline const otype *name() const { \
                        return (enItemType != jft_type) ? NULL : \
                            reinterpret_cast<const otype *>(pData); \
                    }

                __GET_DEF(byte_t, get_bytes, JFT_BYTE)
                __GET_DEF(short_t, get_shorts, JFT_SHORT)
                __GET_DEF(int_t, get_ints, JFT_INTEGER)
                __GET_DEF(long_t, get_longs, JFT_LONG)
                __GET_DEF(float_t, get_floats, JFT_FLOAT)
                __GET_DEF(double_t, get_doubles, JFT_DOUBLE)
                __GET_DEF(bool_t, get_bools, JFT_BOOL)
                __GET_DEF(char_t, get_chars, JFT_CHAR)
                __GET_DEF(Object *, get_objects, JFT_OBJECT)

                #undef __GET_DEF
        };
    
    } /* namespace java */
} /* namespace lsp */

#endif /* CORE_FILES_JAVA_RAWARRAY_H_ */
