/*
 * Enum.h
 *
 *  Created on: 5 сент. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_JAVA_ENUM_H_
#define CORE_FILES_JAVA_ENUM_H_

#include <core/files/java/Object.h>
#include <core/LSPString.h>

namespace lsp
{
    namespace java
    {
        
        class Enum: public Object
        {
            public:
                static const char *CLASS_NAME;

            private:
                friend class ObjectStream;
                Enum & operator = (const Enum &);

            private:
                LSPString   sName;

            protected:
                virtual status_t to_string_padded(LSPString *dst, size_t pad);

            public:
                explicit Enum();
                virtual ~Enum();

            public:
                const LSPString *name() const   { return &sName; }
                const char *utf_name() const    { return sName.get_utf8(); }

                virtual bool instanceof(const char *name) const;
        };
    
    } /* namespace java */
} /* namespace lsp */

#endif /* CORE_FILES_JAVA_ENUM_H_ */
