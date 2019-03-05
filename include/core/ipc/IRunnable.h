/*
 * IRunnable.h
 *
 *  Created on: 5 мар. 2019 г.
 *      Author: sadko
 */

#ifndef INCLUDE_CORE_IPC_IRUNNABLE_H_
#define INCLUDE_CORE_IPC_IRUNNABLE_H_

#include <core/status.h>

namespace lsp
{
    namespace ipc
    {
        /**
         * Runnable interface
         */
        class IRunnable
        {
            private:
                IRunnable & operator = (const IRunnable &src);      // Deny copying

            public:
                explicit IRunnable();
                virtual ~IRunnable();

            public:
                /**
                 * The main method of the runnable interface
                 * @return status of operation
                 */
                virtual status_t run();
        };
    
    } /* namespace ipc */
} /* namespace lsp */

#endif /* INCLUDE_CORE_IPC_IRUNNABLE_H_ */
