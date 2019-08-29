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
            private:
                String & operator = (const String &);

            private:
                LSPString   sString;

            public:
                explicit String(handle_t handle);
                virtual ~String();

            public:
                inline LSPString *string() { return &sString; }

                inline operator LSPString &() { return sString; }
                inline operator LSPString *() { return &sString; }

                inline operator const LSPString &() const { return sString; }
                inline operator const LSPString *() const { return &sString; }
        };
    
    } /* namespace java */
} /* namespace lsp */

#endif /* CORE_FILES_JAVA_JAVASTRING_H_ */
