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

        /**
         * This is common Java object implementation,
         * should be always allocated with new() operator
         */
        class Object
        {
            private:
                Object & operator = (const Object &);

            private:
                ssize_t         nReferences;
                handle_t        nHandle;
                const char     *pClass;

            public:
                explicit Object(handle_t handle, const char *class_name);
                virtual ~Object();

            public:
                inline size_t references() const    { return nReferences; }

                inline const char *class_name() const { return pClass; }

                inline handle_t handle() const       { return nHandle; }

                bool instanceof(const char *name);

                ssize_t acquire();

                ssize_t release();
        };
    
    } /* namespace java */
} /* namespace lsp */

#endif /* CORE_FILES_JAVA_OBJECT_H_ */
