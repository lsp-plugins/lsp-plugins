/*
 * NativeExecutor.cpp
 *
 *  Created on: 27 янв. 2016 г.
 *      Author: sadko
 */

#include <core/debug.h>
#include <core/ipc/NativeExecutor.h>

namespace lsp
{
    namespace ipc
    {
        NativeExecutor::NativeExecutor():
            hThread(execute, this)
        {
            // Initialize list
            pHead       = NULL;
            pTail       = NULL;
            atomic_init(nLock);
        }

        NativeExecutor::~NativeExecutor()
        {
        }

        status_t NativeExecutor::start()
        {
            return hThread.start();
        }

        bool NativeExecutor::submit(ITask *task)
        {
            lsp_trace("submit task=%p", task);
            // Check task state
            if (!task->idle())
                return false;

            // Try to acquire critical section
            if (!atomic_trylock(nLock))
                return false;

            // Update task state to SUBMITTED
            change_task_state(task, ITask::TS_SUBMITTED);

            // Critical section acquired, bind new task
            // Check that queue is empty
            if (pTail != NULL)
                link_task(pTail, task);
            else
                pHead   = task;
            pTail   = task;

            // Release critical section
            atomic_unlock(nLock);
            return true;
        }

        void NativeExecutor::shutdown()
        {
            lsp_trace("start shutdown");

            // Wait until the queue is empty
            while (true)
            {
                // Try to acquire critical section
                if (atomic_trylock(nLock))
                {
                    // Check that queue is empty
                    if (pHead == NULL)
                        break;
                    // Release critical section
                    atomic_unlock(nLock);
                }

                ipc::Thread::sleep(100);
            }

            // Now there are no pending tasks, terminate thread
            hThread.cancel();
            hThread.join();

            lsp_trace("shutdown complete");
        }

        void NativeExecutor::run()
        {
            while (!ipc::Thread::is_cancelled())
            {
                // Sleep until critical section is acquired
                while (!atomic_trylock(nLock))
                {
                    if (ipc::Thread::sleep(100) == STATUS_CANCELLED)
                        return;
                }

                // Try to get task
                ITask  *task    = pHead;
                if (task == NULL)
                {
                    // Release critical section
                    atomic_unlock(nLock);

                    // Wait for a while
                    if (ipc::Thread::sleep(100) == STATUS_CANCELLED)
                        return;
                }
                else
                {
                    // Remove task from queue
                    pHead           = next_task(pHead);
                    if (pHead == NULL)
                        pTail           = NULL;

                    // Release critical section
                    atomic_unlock(nLock);

                    // Execute task
                    lsp_trace("executing task %p", task);
                    run_task(task);
                    lsp_trace("executed task %p with code %d", task, int(task->code()));
                }
            }
        }

        status_t NativeExecutor::execute(void *params)
        {
            NativeExecutor *_this = reinterpret_cast<NativeExecutor *>(params);
            _this->run();
            return STATUS_OK;
        }
    }
}
