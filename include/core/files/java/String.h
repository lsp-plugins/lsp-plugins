/*
 * JavaString.h
 *
 *  Created on: 29 авг. 2019 г.
 *      Author: sadko
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
