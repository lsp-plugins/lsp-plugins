/*
 * Object.h
 *
 *  Created on: 29 авг. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_JAVA_OBJECT_H_
#define CORE_FILES_JAVA_OBJECT_H_

#include <common/types.h>

namespace lsp
{
    namespace java
    {
        typedef uint32_t            handle_t;

        class ObjectStreamClass;

        typedef struct object_slot_t
        {
            ObjectStreamClass      *desc;   // Slot descriptor
            size_t                  flags;  // Flags
            size_t                  offset; // Offset from data beginning
            size_t                  size;   // Size in bytes
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
                Object & operator = (const Object &);

            private:
                const char     *pClass;
                object_slot_t  *vSlots;
                size_t          nSlots;
                uint8_t        *vData;

            public:
                explicit Object(const char *class_name);
                virtual ~Object();

            public:
                inline const char *class_name() const { return pClass; }

                bool instanceof(const char *name);
        };

    } /* namespace java */
} /* namespace lsp */

#endif /* CORE_FILES_JAVA_OBJECT_H_ */
