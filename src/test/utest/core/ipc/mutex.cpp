/*
 * mutex.cpp
 *
 *  Created on: 25 февр. 2019 г.
 *      Author: sadko
 */

#include <core/alloc.h>
#include <test/utest.h>
#include <core/ipc/Thread.h>
#include <core/ipc/Mutex.h>

using namespace lsp;

UTEST_BEGIN("core.ipc", mutex)

    status_t execute(int *shared, ipc::Mutex *mutex)
    {
        UTEST_ASSERT(mutex->lock());
        int value = ++(*shared);

        for (size_t i=0; i<3; ++i)
            UTEST_ASSERT(mutex->lock());

        ipc::Thread::sleep((rand() % 1000) + 100);

        for (size_t i=0; i<3; ++i)
            UTEST_ASSERT(mutex->unlock());

        UTEST_ASSERT(value == (*shared)++);
        UTEST_ASSERT(mutex->unlock());

        return 1;
    }

    class TestThread: public ipc::Thread
    {
        private:
            test_type_t *test;
            ipc::Mutex *mutex;
            int *shared;

        public:
            explicit TestThread() { test = NULL; shared = NULL; mutex = NULL; }
            virtual ~TestThread() {}

            void bind(test_type_t *test, int *shared, ipc::Mutex *mutex)
            {
                this->test = test;
                this->shared = shared;
                this->mutex = mutex;
            }

            virtual status_t run()
            {
                return test->execute(shared, mutex);
            }
    };


    UTEST_MAIN
    {
        TestThread t[4];
        ipc::Mutex mutex;
        int shared = 0;

        printf("Starting threads...\n");
        for (size_t i=0; i<4; ++i)
        {
            t[i].bind(this, &shared, &mutex);
            UTEST_ASSERT(t[i].start() == STATUS_OK);
        }

        printf("Waiting threads for termination...\n");
        for (size_t i=0; i<4; ++i)
            t[i].join();

        printf("Analyzing exit status...\n");
        for (int i=0; i<4; ++i)
        {
            UTEST_ASSERT(!t[i].cancelled());
            UTEST_ASSERT(t[i].get_result() > 0);
        }

        UTEST_ASSERT(shared == 8);
    }
UTEST_END;





