/*
 * executor.cpp
 *
 *  Created on: 19 мар. 2019 г.
 *      Author: sadko
 */

#include <core/alloc.h>
#include <test/utest.h>
#include <core/ipc/Thread.h>
#include <core/ipc/NativeExecutor.h>

using namespace lsp;

static const status_t statuses[] =
{
    STATUS_OK, STATUS_NOT_FOUND, STATUS_BAD_ARGUMENTS, STATUS_CANCELLED
};

UTEST_BEGIN("core.ipc", executor)

    class TestTask: public ipc::ITask
    {
        private:
            size_t nDelay;
            status_t nResult;

        public:
            explicit TestTask(size_t delay, status_t result) : nDelay(delay), nResult(result) {}
            virtual ~TestTask() {}

        public:
            virtual status_t run()
            {
                ipc::Thread::sleep(nDelay);
                return nResult;
            }
    };

    UTEST_MAIN
    {
        TestTask *tasks[4];

        for (size_t i=0; i<4; ++i)
        {
            tasks[i] = new TestTask(100 + (rand() % 100), statuses[i]);
            UTEST_ASSERT(tasks[i] != NULL);
            UTEST_ASSERT(tasks[i]->idle());
        }

        printf("Starting native executor...\n");
        ipc::NativeExecutor executor;
        UTEST_ASSERT(executor.start() == STATUS_OK);

        printf("Submitting tasks...\n");
        for (size_t i=0; i<4; ++i)
        {
            executor.submit(tasks[i]);
            ipc::ITask::task_state_t ts = tasks[i]->state();
            UTEST_ASSERT(
                    (ts == ipc::ITask::TS_SUBMITTED) ||
                    (ts == ipc::ITask::TS_RUNNING)
                    );
        }

        printf("Shuttind down executor...\n");
        executor.shutdown();

        printf("Checking tasks...\n");
        for (size_t i=0; i<4; ++i)
        {
            UTEST_ASSERT(tasks[i]->completed());
            UTEST_ASSERT(tasks[i]->code() == statuses[i]);
            UTEST_ASSERT(tasks[i]->reset());
            UTEST_ASSERT(tasks[i]->idle());
        }

        printf("Destroying tasks...\n");
        for (size_t i=0; i<4; ++i)
            delete tasks[i];
    }

UTEST_END

