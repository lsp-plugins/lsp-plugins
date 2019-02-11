/*
 * executor.h
 *
 *  Created on: 11 февр. 2019 г.
 *      Author: sadko
 */

#ifndef INCLUDE_CONTAINER_TEST_EXECUTOR_H_
#define INCLUDE_CONTAINER_TEST_EXECUTOR_H_

#include <container/test/types.h>
#include <container/test/config.h>
#include <data/cstorage.h>
#include <errno.h>

#if defined(PLATFORM_WINDOWS)
    #include <processthreadsapi.h>
#else
    #include <unistd.h>
#endif

namespace lsp
{
    class TestExecutor
    {
        protected:
            typedef struct task_t
            {
#if defined(PLATFORM_WINDOWS)
                PROCESS_INFORMATION pid;
#else /* POSIX */
                pid_t               pid;
#endif /* PLATFORM_WINDOWS */
                struct timespec     submitted;
                test::Test         *test;
                status_t            result;
            } task_t;

        private:
            size_t              nTotal;
            size_t              nTasksMax;
            size_t              nTasksActive;
            double              fOverall;
            task_t             *vTasks;
            const config_t     *pCfg;

            cvector<test::Test> success;     // List of failed tests
            cvector<test::Test> failed;     // List of failed tests
            cvector<test::Test> ignored;    // List of ignored tests

        protected:
            status_t    launch_test(test::Test *test);
            status_t    wait_for_child();

        public:
            explicit TestExecutor()
            {
                nTotal          = 0;
                nTasksMax       = 0;
                nTasksActive    = 0;
                fOverall        = 0.0f;
                vTasks          = NULL;
                pCfg            = NULL;
            }

            ~TestExecutor()
            {
                wait();
            }

        public:
            /**
             * Configure test launcher
             * @param config launcher configuration
             * @return status of operation
             */
            status_t init(const config_t *config)
            {
                if (config->fork)
                {
                    size_t threads  = (config->threads > 0) ? config->threads : 1;
                    task_t *tasks   = new task_t[threads];
                    if (tasks == NULL)
                        return STATUS_NO_MEM;

                    nTasksMax       = threads;
                    nTasksActive    = 0;
                }
                else
                {
                    nTasksMax       = 0;
                    nTasksActive    = 0;
                    vTasks          = NULL;
                }

                pCfg        = config;

                return STATUS_OK;
            }

            /**
             * Wait for completion of all child processes
             * @return  status of operation
             */
            status_t wait();

            /**
             * Submit test for execution
             * @param test test for execution
             * @return status of operation
             */
            status_t submit(test::Test *test);
    };

    status_t TestExecutor::launch_test(test::Test *test)
    {
        return STATUS_OK;
    }

    status_t TestExecutor::wait()
    {
        while (nTasksActive >= nTasksMax)
        {
            status_t res    = wait_for_child();
            if (res != STATUS_OK)
                return res;
        }

        return STATUS_OK;
    }

    status_t TestExecutor::submit(test::Test *test)
    {
        // Do we need to fork() ?
        if ((!pCfg->fork) || (vTasks == NULL) || (nTasksMax <= 0))
            return launch_test(test);

        // Wait for empty task descriptor
        while (nTasksActive >= nTasksMax)
        {
            status_t res    = wait_for_child();
            if (res != STATUS_OK)
                return res;
        }

        fflush(stdout);
        fflush(stderr);

        // Allocate new task descriptor
        task_t *task        = &vTasks[nTasksActive++];
        clock_gettime(CLOCK_REALTIME, &task->submitted);
        task->test          = test;
        task->result        = STATUS_OK;

        // Launch the nested process
#if defined(PLATFORM_WINDOWS)
        // TODO
#else
        pid_t pid = fork();
        if (pid == 0)
            return launch_test(test);
        else if (pid < 0)
        {
            int error = errno;
            fprintf(stderr, "Error while spawning child process %d\n", error);
            fflush(stderr);
            return STATUS_UNKNOWN_ERR;
        }

        task->pid       = pid;
#endif /* PLATFORM_WINDOWS */

        return STATUS_OK;
    }

    status_t TestExecutor::wait_for_child()
    {
        return STATUS_OK;
    }
}


#endif /* INCLUDE_CONTAINER_TEST_EXECUTOR_H_ */
