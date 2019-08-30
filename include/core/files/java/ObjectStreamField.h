/*
 * ObjectStreamField.h
 *
 *  Created on: 30 авг. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_JAVA_OBJECTSTREAMFIELD_H_
#define CORE_FILES_JAVA_OBJECTSTREAMFIELD_H_

#include <core/LSPString.h>
#include <core/files/java/Object.h>
#include <core/files/java/String.h>

namespace lsp
{
    namespace java
    {
        enum ftype_t
        {
            JFT_BYTE,
            JFT_CHAR,
            JFT_DOUBLE,
            JFT_FLOAT,
            JFT_INTEGER,
            JFT_LONG,
            JFT_SHORT,
            JFT_BOOL,
            JFT_ARRAY,
            JFT_OBJECT,

            JFT_TOTAL,
            JFT_UNKNOWN = -1
        };
        
        class ObjectStreamField: public Object
        {
            public:
                static const char *CLASS_NAME;

            private:
                friend class ObjectStream;
                ObjectStreamField & operator = (const ObjectStreamField &);

            protected:
                ftype_t         enType;
                LSPString       sName;
                String         *pSignature;
                size_t          nOffset;

            public:
                explicit ObjectStreamField();
                virtual ~ObjectStreamField();

            public:
                inline ftype_t              type() const { return enType; }
                inline const LSPString     *name() const { return &sName; }
                inline const LSPString     *signature() const { return (pSignature != NULL) ? pSignature->string() : NULL; }
                inline const String        *java_signature() const { return pSignature; }
                size_t                      size_of() const;
                inline bool                 is_reference() const { return (enType == JFT_ARRAY) || (enType == JFT_OBJECT); }
        };
    
    } /* namespace java */
} /* namespace lsp */

#endif /* CORE_FILES_JAVA_OBJECTSTREAMFIELD_H_ */
