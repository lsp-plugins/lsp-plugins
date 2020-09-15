/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 29 авг. 2019 г.
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

#ifndef CORE_FILES_JAVA_JAVASTRING_H_
#define CORE_FILES_JAVA_JAVASTRING_H_

#include <core/files/java/Object.h>
#include <core/LSPString.h>

namespace lsp
{
    namespace java
    {
        /**
         * String implementation of Java object
         */
        class String: public Object
        {
            public:
                static const char *CLASS_NAME;

            private:
                String & operator = (const String &);

            private:
                LSPString   sString;

            protected:
                virtual status_t to_string_padded(LSPString *dst, size_t pad);

            public:
                explicit String();
                virtual ~String();

            public:
                inline LSPString *string() { return &sString; }
                inline const LSPString *string() const { return &sString; }

                inline operator LSPString &() { return sString; }
                inline operator LSPString *() { return &sString; }

                inline operator const LSPString &() const { return sString; }
                inline operator const LSPString *() const { return &sString; }
        };
    
    } /* namespace java */
} /* namespace lsp */

#endif /* CORE_FILES_JAVA_JAVASTRING_H_ */
