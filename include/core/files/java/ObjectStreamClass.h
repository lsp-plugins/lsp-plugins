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
#include <core/files/java/const.h>
#include <core/files/java/Object.h>
#include <core/files/java/ObjectStreamField.h>

namespace lsp
{
    namespace java
    {
        class ObjectStreamClass: public Object
        {
            public:
                static const char *CLASS_NAME;

            private:
                friend class ObjectStream;
                ObjectStreamClass & operator = (const ObjectStreamClass &);

            protected:
                ObjectStreamClass  *pParent;
                LSPString           sName;
                char               *pRawName;
                uint64_t            nSuid;
                size_t              nFlags;
                size_t              nFields;
                size_t              nSlots;
                size_t              nSizeOf;
                ObjectStreamField **vFields;
                ObjectStreamClass **vSlots;

            public:
                explicit ObjectStreamClass();
                virtual ~ObjectStreamClass();

            public:
                inline ObjectStreamClass *parent()          { return pParent; }
                inline const LSPString *name() const        { return &this->sName; }
                inline const char *raw_name() const         { return pRawName; }
                inline size_t size_of() const               { return nSizeOf; }
                inline uint64_t suid() const                { return this->nSuid;  }
                inline bool is_proxy() const                { return nFlags & JCF_PROXY; }
                inline bool has_write_method() const        { return nFlags & JCF_WRITE_METHOD; }
                inline bool has_block_external_data() const { return nFlags & JCF_BLOCK_DATA; }
                inline bool is_externalizable() const       { return nFlags & JCF_EXTERNALIZABLE; }
                inline bool is_serializable() const         { return nFlags & (JCF_EXTERNALIZABLE | JCF_SERIALIZABLE); }
                inline const ObjectStreamField *field(size_t idx) const { return (idx < nFields) ? vFields[idx] : NULL; }
                inline const ObjectStreamClass *slot(size_t idx) const { return (idx < nSlots) ? vSlots[idx] : NULL; }
                inline size_t fields() const                { return nFields; };
        };
    
    } /* namespace java */
} /* namespace lsp */

#endif /* CORE_FILES_JAVA_OBJECTSTREAMCLASS_H_ */
