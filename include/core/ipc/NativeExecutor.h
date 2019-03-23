/*
 * NativeExecutor.h
 *
 *  Created on: 27 янв. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_NATIVEEXECUTOR_H_
#define CORE_NATIVEEXECUTOR_H_

#include <dsp/atomic.h>
#include <core/ipc/Thread.h>
#include <core/ipc/IExecutor.h>
#include <core/ipc/ITask.h>

namespace lsp
{
    namespace ipc
    {
        class NativeExecutor: public IExecutor
        {
            private:
                Thread              hThread;
                ITask              *pHead;
                ITask              *pTail;
                atomic_t            nLock;

                static status_t     execute(void *params);
                void    run();

            private:
                NativeExecutor &operator = (const NativeExecutor &src); // Deny copying

            public:
                explicit NativeExecutor();
                virtual ~NativeExecutor();

            public:
                status_t start();

                virtual bool submit(ITask *task);

                virtual void shutdown();
        };
    }
}

#endif /* CORE_NATIVEEXECUTOR_H_ */
