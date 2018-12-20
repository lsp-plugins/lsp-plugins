/*
 * NativeExecutor.cpp
 *
 *  Created on: 27 янв. 2016 г.
 *      Author: sadko
 */

#include <core/debug.h>
#include <core/NativeExecutor.h>

namespace lsp
{
    NativeExecutor::NativeExecutor()
    {
        // Initialize list
        pHead       = NULL;
        pTail       = NULL;
        atomic_init(nLock);

        // Initialize condition variable
        pthread_condattr_t cattr;
        pthread_condattr_init(&cattr);
        pthread_cond_init(&hCond, &cattr);
        pthread_condattr_destroy(&cattr);

        // Initialize thread
        pthread_attr_t pattr;
        pthread_attr_init(&pattr);
        pthread_create(&hThread, &pattr, execute, this);
        pthread_attr_destroy(&pattr);
    }

    NativeExecutor::~NativeExecutor()
    {
        pthread_cond_destroy(&hCond);
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
        struct timespec spec = { 0, 100 * 1000 * 1000 }; // 100 msec
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

            nanosleep(&spec, NULL);
        }

        // Now there are no pending tasks, terminate thread
        pthread_cancel(hThread);
        pthread_join(hThread, NULL);
        hThread = 0;

        lsp_trace("shutdown complete");
    }

    void NativeExecutor::run()
    {
        struct timespec spec = { 0, 100 * 1000 * 1000 }; // 100 msec

        while (true)
        {
            // Sleep until critical section is acquired
            while (!atomic_trylock(nLock))
                nanosleep(&spec, NULL);

            // Try to get task
            ITask  *task    = pHead;
            if (task == NULL)
            {
                // Release critical section
                atomic_unlock(nLock);

                // Wait for a while
                nanosleep(&spec, NULL);
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

    void *NativeExecutor::execute(void *params)
    {
        NativeExecutor *_this = reinterpret_cast<NativeExecutor *>(params);
        _this->run();
        return NULL;
    }
}
