/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 6 сент. 2019 г.
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

#ifndef CORE_FILES_JAVA_WRAPPERS_H_
#define CORE_FILES_JAVA_WRAPPERS_H_

#include <core/files/java/const.h>
#include <core/files/java/Object.h>

namespace lsp
{
    namespace java
    {
        #define WRAPPER_DEF(name, type_t) \
            class name: public Object \
            { \
                public: \
                    static const char *CLASS_NAME; \
                \
                protected: \
                    virtual status_t to_string_padded(LSPString *dst, size_t pad); \
                \
                public: \
                    explicit name(); \
                    virtual ~name(); \
                \
                public: \
                    type_t value() const; \
                    status_t get_value(type_t *dst) const; \
            }

        WRAPPER_DEF(Byte, byte_t);
        WRAPPER_DEF(Short, short_t);
        WRAPPER_DEF(Integer, int_t);
        WRAPPER_DEF(Long, long_t);
        WRAPPER_DEF(Double, double_t);
        WRAPPER_DEF(Float, float_t);
        WRAPPER_DEF(Boolean, bool_t);
        WRAPPER_DEF(Character, char_t);

        #undef WRAPPER_DEF
    }
}


#endif /* CORE_FILES_JAVA_WRAPPERS_H_ */
