/*
 * thread.cpp
 *
 *  Created on: 25 февр. 2019 г.
 *      Author: sadko
 */

#include <core/alloc.h>
#include <test/utest.h>
#include <core/ipc/Thread.h>

using namespace lsp;

UTEST_BEGIN("core.ipc", thread)
    class TestThread: public ipc::Thread
    {
        private:
            volatile int *res;
            int exit_code;

        public:
            explicit TestThread() { res = NULL; exit_code = -1; }
            virtual ~TestThread() {}

            void bind(volatile int *res, int exit_code) { this->res = res; this->exit_code = exit_code; }

            int run()
            {
                while (*res == 0)
                    Thread::sleep(100);

                *res    = 2;
                Thread::sleep(5000);
                *res    = 3;

                return exit_code;
            }
    };

    UTEST_MAIN
    {
        TestThread t[4];
        volatile int res[4];

        printf("Starting threads...\n");
        for (size_t i=0; i<4; ++i)
        {
            res[i] = 0;
            t[i].bind(&res[i], i+1);
            t[i].start();
        }

        printf("Sleeping...\n");
        ipc::Thread::sleep(1000);

        printf("Waking threads...\n");
        for (size_t i=0; i<4; ++i)
        {
            UTEST_ASSERT(res[i] == 0);
            res[i] = 1;
        }

        printf("Syncing...\n");
        while (true)
        {
            size_t n = 0;
            for (size_t i=0; i<4; ++i)
            {
                UTEST_ASSERT(res[i] < 2);
                if (res[i] == 1)
                    ++n;
            }
            if (n == 4)
                break;
        }

        printf("Cancelling threads...\n");
        for (size_t i=0; i<4; ++i)
        {
            UTEST_ASSERT(!t[i].cancelled());
            t[i].cancel();
        }

        printf("Waiting threads for termination...\n");
        for (size_t i=0; i<4; ++i)
            t[i].join();

        printf("Analyzing exit status...\n");
        for (int i=0; i<4; ++i)
        {
            UTEST_ASSERT(t[i].cancelled());
            UTEST_ASSERT(t[i].get_result() == (i+1));
        }
    }
UTEST_END;


