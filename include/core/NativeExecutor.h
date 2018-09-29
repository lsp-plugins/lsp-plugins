/*
 * NativeExecutor.h
 *
 *  Created on: 27 янв. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_NATIVEEXECUTOR_H_
#define CORE_NATIVEEXECUTOR_H_

#include <core/IExecutor.h>
#include <core/ITask.h>
#include <dsp/atomic.h>
#include <pthread.h>

namespace lsp
{
    class NativeExecutor: public IExecutor
    {
        private:
            pthread_t           hThread;
            pthread_cond_t      hCond;
            ITask              *pHead;
            ITask              *pTail;
            atomic_t            nLock;

            static void *execute(void *params);
            void    run();

        public:
            NativeExecutor();
            virtual ~NativeExecutor();

        public:
            virtual bool submit(ITask *task);

            virtual void shutdown();
    };
}

#endif /* CORE_NATIVEEXECUTOR_H_ */
