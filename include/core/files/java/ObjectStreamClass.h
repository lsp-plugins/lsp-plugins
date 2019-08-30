/*
 * ObjectStreamClass.h
 *
 *  Created on: 29 авг. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_JAVA_OBJECTSTREAMCLASS_H_
#define CORE_FILES_JAVA_OBJECTSTREAMCLASS_H_

#include <common/types.h>
#include <core/LSPString.h>
#include <core/files/java/Object.h>
#include <core/files/java/ObjectStreamField.h>

namespace lsp
{
    namespace java
    {
        enum cflags_t
        {
            JCF_PROXY           = 1 << 0,
            JCF_WRITE_METHOD    = 1 << 1,
            JCF_BLOCK_DATA      = 1 << 2,
            JCF_EXTERNALIZABLE  = 1 << 3,
            JCF_SERIALIZABLE    = 1 << 4,
            JCF_ENUM            = 1 << 5
        };

        class ObjectStreamClass: public Object
        {
            public:
                static const char *CLASS_NAME;

            private:
                friend class ObjectStream;
                ObjectStreamClass & operator = (const ObjectStreamClass &);

            protected:
                LSPString           sName;
                uint64_t            nSuid;
                size_t              nFlags;
                size_t              nFields;
                ObjectStreamField **vFields;

            public:
                explicit ObjectStreamClass();
                virtual ~ObjectStreamClass();

            public:
                inline const LSPString *name() const        { return &this->sName; }
                inline uint64_t suid() const                { return this->nSuid;  }
                inline bool is_proxy() const                { return nFlags & JCF_PROXY; }
                inline bool has_write_method() const        { return nFlags & JCF_WRITE_METHOD; }
                inline bool has_block_external_data() const { return nFlags & JCF_BLOCK_DATA; }
                inline bool is_externalizable() const       { return nFlags & JCF_EXTERNALIZABLE; }
                inline bool is_serializable() const         { return nFlags & (JCF_EXTERNALIZABLE | JCF_SERIALIZABLE); }
        };
    
    } /* namespace java */
} /* namespace lsp */

#endif /* CORE_FILES_JAVA_OBJECTSTREAMCLASS_H_ */
