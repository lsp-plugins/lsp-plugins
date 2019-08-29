/*
 * ClassDescriptor.h
 *
 *  Created on: 29 авг. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_JAVA_CLASSDESCRIPTOR_H_
#define CORE_FILES_JAVA_CLASSDESCRIPTOR_H_

#include <common/types.h>

namespace lsp
{
    namespace java
    {
        class ClassDescriptor
        {
            private:
                size_t      nHandle;

            public:
                explicit ClassDescriptor(size_t handle);
                virtual ~ClassDescriptor();

            public:
                inline uint32_t     handle() const { return nHandle; }


        };
    
    } /* namespace java */
} /* namespace lsp */

#endif /* CORE_FILES_JAVA_CLASSDESCRIPTOR_H_ */
