/*
 * executor.h
 *
 *  Created on: 11 февр. 2019 г.
 *      Author: sadko
 */

#ifndef TEST_MAIN_EXECUTOR_H_
#define TEST_MAIN_EXECUTOR_H_

#include <test/main/types.h>
#include <test/main/config.h>
#include <core/io/charset.h>
#include <errno.h>

namespace lsp
{
    class TestExecutor
    {
        protected:
            typedef struct task_t
            {
                test_pid_t          pid;
                test_clock_t        submitted;
                test::Test         *test;
                status_t            result;
            } task_t;

        private:
            size_t              nTotal;
            size_t              nTasksMax;
            size_t              nTasksActive;
            double              fOverall;
            task_t             *vTasks;
            config_t           *pCfg;
            stats_t            *pStats;
#ifdef PLATFORM_WINDOWS
            HANDLE              hThread;
            HANDLE              hTimer;
            HANDLE              hLock;
#endif /* PLATFORM_WINDOWS */

        protected:
            status_t    launch_test(test::Test *test);
            status_t    wait_for_children();
            status_t    launch(test::UnitTest *test);
            status_t    launch(test::PerformanceTest *test);
            status_t    launch(test::ManualTest *test);

            // Platform-dependent routines
            status_t    submit_task(task_t *task);
            status_t    wait_for_child(task_t **task);
            status_t    set_timeout(double timeout);
            status_t    kill_timeout();
            void        start_memcheck(test::Test *test);
            void        end_memcheck();

#ifdef PLATFORM_WINDOWS
            static DWORD WINAPI  thread_proc(LPVOID params);
#endif /* PLATFORM_WINDOWS */

        public:
            explicit TestExecutor()
            {
                nTotal          = 0;
                nTasksMax       = 0;
                nTasksActive    = 0;
                fOverall        = 0.0f;
                vTasks          = NULL;
                pCfg            = NULL;
                pStats          = NULL;

#ifdef PLATFORM_WINDOWS
                hThread         = 0;
                hTimer          = 0;
                hLock           = 0;
#endif /* PLATFORM_WINDOWS */
            }

            ~TestExecutor()
            {
                wait();
            }

        public:
            /**
             * Configure test launcher
             * @param config launcher configuration
             * @param stats pointer to statistics structure
             * @return status of operation
             */
            status_t init(config_t *config, stats_t *stats);

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

    status_t TestExecutor::init(config_t *config, stats_t *stats)
    {
#if defined(PLATFORM_WINDOWS)
        SetErrorMode(SEM_NOGPFAULTERRORBOX | SEM_FAILCRITICALERRORS);
#endif /* PLATFORM_WINDOWS */

        if (config->fork)
        {
            size_t threads  = (config->threads > 0) ? config->threads : 1;
            if (config->mode != UTEST)
                threads         = 1;

            task_t *tasks   = new task_t[threads];
            if (tasks == NULL)
                return STATUS_NO_MEM;

            nTasksMax       = threads;
            nTasksActive    = 0;
            vTasks          = tasks;
        }
        else
        {
            nTasksMax       = 0;
            nTasksActive    = 0;
            vTasks          = NULL;
        }

        pCfg        = config;
        pStats      = stats;

        return STATUS_OK;
    }

    status_t TestExecutor::wait()
    {
        if (pCfg->is_child)
            return STATUS_OK;

        while (nTasksActive > 0)
        {
            status_t res    = wait_for_children();
            if (res != STATUS_OK)
                return res;
        }

        return STATUS_OK;
    }

    status_t TestExecutor::submit(test::Test *test)
    {
        const char *tclass  = (pCfg->mode == UTEST) ? "unit test" :
                              (pCfg->mode == PTEST) ? "performance test" :
                              "manual test";

        // Do we need to fork() ?
        if ((!pCfg->fork) || (vTasks == NULL) || (nTasksMax <= 0))
        {
            status_t res = launch_test(test);
            if (pStats != NULL)
            {
                if (res == STATUS_OK)
                    pStats->success.add(test);
                else
                    pStats->failed.add(test);
            }
            return res;
        }

        // Wait for empty task descriptor
        while (nTasksActive >= nTasksMax)
        {
            status_t res    = wait_for_children();
            if (res != STATUS_OK)
                return res;
        }

        if (!pCfg->is_child)
        {
            printf("\n--------------------------------------------------------------------------------\n");
            printf("Launching %s '%s'\n", tclass, test->full_name());
            printf("--------------------------------------------------------------------------------\n");
        }

        fflush(stdout);
        fflush(stderr);

        // Allocate new task descriptor
        task_t *task        = &vTasks[nTasksActive++];
        get_test_time(&task->submitted); // Remember start time of the test
        task->test          = test;
        task->result        = STATUS_OK;

        // Launch the nested process
        status_t res        = submit_task(task);
        if (res != STATUS_OK)
            --nTasksActive;
        return res;
    }

    status_t TestExecutor::wait_for_children()
    {
        test_clock_t ts;
        const char *test    = (pCfg->mode == UTEST) ? "Unit test" :
                              (pCfg->mode == PTEST) ? "Performance test" :
                              "Manual test";

        // Try to wait for child task
        task_t *task        = NULL;
        status_t res        = wait_for_child(&task);
        if ((res != STATUS_OK) || (task == NULL))
            return res;

        // Get execution time
        get_test_time(&ts);
        double time = calc_test_time_difference(&task->submitted, &ts);
        printf("%s '%s' has %s, execution time: %.2f s\n",
                test, task->test->full_name(), (task->result == 0) ? "succeeded" : "failed", time);

        // Update statistics
        if (pStats != NULL)
        {
            if (task->result == STATUS_OK)
                pStats->success.add(task->test);
            else
                pStats->failed.add(task->test);
        }

        // Free task descriptor
        if (task < &vTasks[--nTasksActive])
            *task   = vTasks[nTasksActive];

        return STATUS_OK;
    }

    status_t TestExecutor::launch(test::UnitTest *test)
    {
        // Set-up timer for deadline expiration
        status_t res = STATUS_OK;
        if (!pCfg->debug)
            res = set_timeout(test->time_limit());

        // Launch unit test
        if (res == STATUS_OK)
        {
            config_t *cfg = const_cast<config_t *>(pCfg);

            test->set_executable(pCfg->executable);
            test->set_verbose(pCfg->verbose);
            start_memcheck(test);
            test->init();
            test->execute(pCfg->args.size(), const_cast<const char **>(cfg->args.get_array()));
            test->destroy();
            end_memcheck();
        }

        // Cancel and disable timer
        if ((res == STATUS_OK) && (!pCfg->debug))
        {
            status_t res = kill_timeout();
            if (res != STATUS_OK)
                return res;
        }

        // Return success
        return STATUS_OK;
    }

    status_t TestExecutor::launch(test::PerformanceTest *test)
    {
        config_t *cfg = const_cast<config_t *>(pCfg);

        // Execute performance test
        test->set_executable(pCfg->executable);
        test->set_verbose(pCfg->verbose);
        start_memcheck(test);
        test->init();
        test->execute(pCfg->args.size(), const_cast<const char **>(cfg->args.get_array()));
        test->destroy();
        end_memcheck();

        // Output peformance test statistics
        printf("\nStatistics of performance test '%s':\n", test->full_name());
        test->dump_stats(stdout);

        // Additionally dump performance statistics to output file
        if (pCfg->outfile != NULL)
        {
            FILE *fd = fopen(pCfg->outfile, "a");
            if (fd != NULL)
            {
                fprintf(fd, "--------------------------------------------------------------------------------\n");
                fprintf(fd, "Statistics of performance test '%s':\n\n", test->full_name());
                test->dump_stats(fd);
                fprintf(fd, "\n");
                fflush(fd);
                fclose(fd);
            }
        }

        test->free_stats();

        return STATUS_OK;
    }

    status_t TestExecutor::launch(test::ManualTest *test)
    {
        config_t *cfg = const_cast<config_t *>(pCfg);
        // Execute performance test
        test->set_executable(pCfg->executable);
        test->set_verbose(pCfg->verbose);
        start_memcheck(test);
        test->init();
        test->execute(pCfg->args.size(), const_cast<const char **>(cfg->args.get_array()));
        test->destroy();
        end_memcheck();

        return STATUS_OK;
    }

    status_t TestExecutor::launch_test(test::Test *test)
    {
        switch (pCfg->mode)
        {
            case UTEST:
                return launch(static_cast<test::UnitTest *>(test));
            case PTEST:
                return launch(static_cast<test::PerformanceTest *>(test));
            case MTEST:
                return launch(static_cast<test::ManualTest *>(test));
            default:
                break;
        }
        return STATUS_BAD_STATE;
    }

#ifdef PLATFORM_WINDOWS
    DWORD WINAPI TestExecutor::thread_proc(LPVOID params)
    {
        TestExecutor *_this = reinterpret_cast<TestExecutor *>(params);

        HANDLE wait[2];
        wait[0] = _this->hLock;
        wait[1] = _this->hTimer;

        DWORD res   = WaitForMultipleObjects(2, wait, FALSE, INFINITE);
        // Check wait result
        if (res == WAIT_FAILED)
        {
            fprintf(stderr, "Test execution wait failed: error_code=%d\n", int(GetLastError()));
            fflush(stdout);
            fflush(stderr);
            ExitProcess(STATUS_TIMED_OUT);
        }
        else if (res >= WAIT_ABANDONED_0)
            res    -= WAIT_ABANDONED_0;

        if (res != 0) // Mutex object has triggered?
        {
            fprintf(stderr, "Test has timed out\n");
            fflush(stdout);
            fflush(stderr);
            ExitProcess(STATUS_TIMED_OUT);
        }

        return STATUS_OK;
    }

    status_t TestExecutor::set_timeout(double timeout)
    {
        hLock   = CreateMutexW(NULL, TRUE, L"timeout_handler");
        if (!hLock)
            return STATUS_UNKNOWN_ERR;

        hTimer  = CreateWaitableTimerW(NULL, TRUE, L"timeout_countdown");
        if (!hTimer)
        {
            CloseHandle(hLock);
            return STATUS_UNKNOWN_ERR;
        }

        LARGE_INTEGER liDueTime;
        liDueTime.QuadPart  = int64_t(-1e+7 * timeout); // 100 nsec intervals
        if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0))
        {
            CloseHandle(hTimer);
            CloseHandle(hLock);
            return STATUS_UNKNOWN_ERR;
        }

        DWORD tid;
        hThread = CreateThread(NULL, 0, thread_proc, this, 0, &tid);
        if (!hThread)
        {
            CloseHandle(hTimer);
            CloseHandle(hLock);
            return STATUS_UNKNOWN_ERR;
        }

        return STATUS_OK;
    }

    status_t TestExecutor::kill_timeout()
    {
        ReleaseMutex(hLock);
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
        CloseHandle(hTimer);
        CloseHandle(hLock);

        hThread     = 0;
        hTimer      = 0;
        hLock       = 0;

        return STATUS_OK;
    }

    status_t cmdline_append_char(char **buffer, size_t *length, size_t *capacity, char ch)
    {
        char *dst           = *buffer;
        dst[(*length)++]    = ch; // We have at least 1 character at the tail ('\0')

        if (*length < *capacity)
            return STATUS_OK;

        *capacity   = (*capacity > 0) ? ((*capacity) << 1) : 32;
        dst         = reinterpret_cast<char *>(realloc(dst, *capacity + 1)); // Do not count the last '\0' character as capacity
        if (dst == NULL)
            return STATUS_NO_MEM;
        *buffer     = dst;

        return STATUS_OK;
    }

    status_t cmdline_append_escaped(char **buffer, size_t *length, size_t *capacity, const char *text, bool space=true)
    {
        char ch;
        status_t res    = STATUS_OK;

        // Append space if needed
        if (space)
        {
            res = cmdline_append_char(buffer, length, capacity, ' ');
            if (res != STATUS_OK)
                return res;
        }

        // Check argument length
        if (*text == '\0')
        {
            // Empty argument
            res = cmdline_append_char(buffer, length, capacity, '\"');
            if (res == STATUS_OK)
                res = cmdline_append_char(buffer, length, capacity, '\"');
        }
        else
        {
            // Non-empty argument, process it:
            //   ' ' -> '\"', ' ', '\"'
            //   '"' -> '\\', '\"'
            while ((ch = *(text++)) != '\0')
            {
                switch (ch)
                {
                    case ' ':
                        res = cmdline_append_char(buffer, length, capacity, '\"');
                        if (res == STATUS_OK)
                            res = cmdline_append_char(buffer, length, capacity, ' ');
                        if (res == STATUS_OK)
                            res = cmdline_append_char(buffer, length, capacity, '\"');
                        break;
                    case '"':
                        res = cmdline_append_char(buffer, length, capacity, '\\');
                        if (res == STATUS_OK)
                            res = cmdline_append_char(buffer, length, capacity, '\"');
                        break;
                    default:
                        res = cmdline_append_char(buffer, length, capacity, ch);
                        break;
                }

                if (res != STATUS_OK)
                    break;
            }
        }

        // Store end-of-string character
        (*buffer)[*length]  = '\0';

        return res;
    }

    status_t TestExecutor::submit_task(task_t *task)
    {
        status_t res;
        char *cmdbuf    = NULL;
        size_t len      = 0;
        size_t cap      = 0;

        // Append executable name
        res     = cmdline_append_escaped(&cmdbuf, &len, &cap, pCfg->executable, false);
        if (res != STATUS_OK)
            return res;

        // Append parameters
        res     = cmdline_append_escaped(&cmdbuf, &len, &cap,
                (pCfg->mode == UTEST) ? "utest" :
                (pCfg->mode == PTEST) ? "ptest" :
                "mtest"
            );
        if (res == STATUS_OK)
            res     = cmdline_append_escaped(&cmdbuf, &len, &cap, "--run-as-nested-process");
        if (res == STATUS_OK)
            res     = cmdline_append_escaped(&cmdbuf, &len, &cap, "--nosysinfo");
        if (res == STATUS_OK)
            res     = cmdline_append_escaped(&cmdbuf, &len, &cap, "--nofork");
        if ((res == STATUS_OK) && (pCfg->debug))
            res     = cmdline_append_escaped(&cmdbuf, &len, &cap, "--debug");
        if (res == STATUS_OK)
            res     = cmdline_append_escaped(&cmdbuf, &len, &cap, (pCfg->verbose) ? "--verbose" : "--silent");
        if ((res == STATUS_OK) && (pCfg->outfile != NULL))
        {
            res     = cmdline_append_escaped(&cmdbuf, &len, &cap, "--outfile");
            if (res == STATUS_OK)
                res     = cmdline_append_escaped(&cmdbuf, &len, &cap, pCfg->outfile);
        }
        if (res == STATUS_OK)
            res     = cmdline_append_escaped(&cmdbuf, &len, &cap, task->test->full_name());
        if ((res == STATUS_OK) && (pCfg->args.size() > 0))
        {
            res     = cmdline_append_escaped(&cmdbuf, &len, &cap, "--args");
            if (res == STATUS_OK)
            {
                for (size_t i=0, n=pCfg->args.size(); i<n; ++i)
                    if ((res = cmdline_append_escaped(&cmdbuf, &len, &cap, pCfg->args.get(i))) != STATUS_OK)
                        break;
            }
        }

        if (res != STATUS_OK)
        {
            free(cmdbuf);
            return res;
        }

        // Allocate arguments and executable strings
        WCHAR *cmd          = lsp::utf8_to_utf16(cmdbuf);
        free(cmdbuf);
        if (cmd == NULL)
            return STATUS_NO_MEM;
        WCHAR *executable   = lsp::utf8_to_utf16(pCfg->executable);
        if (executable == NULL)
        {
            free(cmd);
            return STATUS_NO_MEM;
        }

        // Launch child process
        STARTUPINFOW si;
        PROCESS_INFORMATION pi;

        ZeroMemory( &si, sizeof(STARTUPINFOW) );
        ZeroMemory( &pi, sizeof(PROCESS_INFORMATION) );
        si.cb = sizeof(si);

        // Start the child process.
        if( !CreateProcessW(
            executable,     // No module name (use command line)
            cmd,            // Command line
            NULL,           // Process handle not inheritable
            NULL,           // Thread handle not inheritable
            FALSE,          // Set handle inheritance to FALSE
            0,              // No creation flags
            NULL,           // Use parent's environment block
            NULL,           // Use parent's starting directory
            &si,            // Pointer to STARTUPINFO structure
            &pi             // Pointer to PROCESS_INFORMATION structure
        ))
        {
            free(cmd);
            free(executable);
            fprintf(stderr, "Failed to create child process (%d)\n", int(GetLastError()));
            fflush(stderr);
            return STATUS_UNKNOWN_ERR;
        }

        free(cmd);
        free(executable);

        task->pid       = pi;
        return STATUS_OK;
    }

    status_t TestExecutor::wait_for_child(task_t **task)
    {
        if (nTasksActive <= 0)
            return STATUS_NOT_FOUND;

        // Wait for any child process exit event
        HANDLE *pids    = reinterpret_cast<HANDLE *>(alloca(nTasksActive * sizeof(HANDLE)));
        for (size_t i=0; i<nTasksActive; ++i)
            pids[i]         = vTasks[i].pid.hProcess;

        size_t idx      = WaitForMultipleObjects(nTasksActive, pids, FALSE, INFINITE);

        // Get termination code
        DWORD code;
        GetExitCodeProcess(pids[idx], &code);
        vTasks[idx].result  = code;
        *task       = &vTasks[idx];

        return STATUS_OK;
    }
#endif /* PLATFORM_WINDOWS */

#ifdef PLATFORM_LINUX
    void TestExecutor::start_memcheck(test::Test *v)
    {
        if (!pCfg->mtrace)
            return;

        // Enable memory trace
        char fname[PATH_MAX];
        mkdir(pCfg->tracepath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        snprintf(fname, PATH_MAX, "%s/%s.utest.mtrace", pCfg->tracepath, v->full_name());
        fname[PATH_MAX-1] = '\0';

        fprintf(stderr, "Enabling memory trace for test '%s' into file '%s'\n", v->full_name(), fname);
        fflush(stderr);

        setenv("MALLOC_TRACE", fname, 1);

        mtrace();
    }

    void TestExecutor::end_memcheck()
    {
        // Disable memory trace
        if (pCfg->mtrace)
            muntrace();

        // Validate heap
//        mcheck_check_all();
    }
#else
    void TestExecutor::start_memcheck(test::Test *v)
    {
    }

    void TestExecutor::end_memcheck()
    {
    }
#endif /* PLATFORM_LINUX */

#ifdef PLATFORM_UNIX_COMPATIBLE
    void utest_timeout_handler(int signum)
    {
        fprintf(stderr, "Unit test time limit exceeded\n");
        exit(STATUS_TIMED_OUT);
    }

    status_t TestExecutor::submit_task(task_t *task)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            pCfg->is_child  = true;
            return launch_test(task->test);
        }
        else if (pid < 0)
        {
            int error = errno;
            fprintf(stderr, "Error while spawning child process %d\n", error);
            fflush(stderr);
            return STATUS_UNKNOWN_ERR;
        }

        task->pid       = pid;
        return STATUS_OK;
    }

    status_t TestExecutor::wait_for_child(task_t **task)
    {
        task_t *ret       = NULL;
        int result;

        do
        {
            pid_t pid = waitpid(-1, &result, WUNTRACED | WCONTINUED);
            if (pid < 0)
            {
                fprintf(stderr, "Child process completion wait failed\n");
                return STATUS_UNKNOWN_ERR;
            }

            // Find the associated thread process
            ret       = NULL;
            for (size_t i=0; i<nTasksActive; ++i)
                if (vTasks[i].pid == pid)
                {
                    ret   = &vTasks[i];
                    break;
                }

            if (WIFSTOPPED(result))
                printf("Child process %d stopped by signal %d\n", int(pid), WSTOPSIG(result));
        } while ((!WIFEXITED(result)) && !WIFSIGNALED(result));

        if (WIFEXITED(result))
            ret->result   = WEXITSTATUS(result);
        else if (WIFSIGNALED(result))
            ret->result   = STATUS_KILLED;

        *task   = ret;
        return STATUS_OK;
    }

    status_t TestExecutor::set_timeout(double timeout)
    {
        struct itimerval timer;

        timer.it_interval.tv_sec    = timeout;
        timer.it_interval.tv_usec   = suseconds_t(timeout * 1e+6) % 1000000L;
        timer.it_value              = timer.it_interval;

        status_t res                = STATUS_OK;
        if (setitimer(ITIMER_REAL, &timer, NULL) != 0)
        {
            int code = errno;
            fprintf(stderr, "setitimer failed with errno=%d\n", code);
            fflush(stderr);
            res = STATUS_UNKNOWN_ERR;
        }
        signal(SIGALRM, utest_timeout_handler);

        return res;
    }

    status_t TestExecutor::kill_timeout()
    {
        struct itimerval timer;

        timer.it_interval.tv_sec    = 0;
        timer.it_interval.tv_usec   = 0;
        timer.it_value              = timer.it_interval;

        signal(SIGALRM, SIG_DFL);
        if (setitimer(ITIMER_REAL, &timer, NULL) == 0)
            return STATUS_OK;

        int code = errno;
        fprintf(stderr, "setitimer failed with errno=%d\n", code);
        fflush(stderr);
        return STATUS_UNKNOWN_ERR;
    }

#endif /* PLATFORM_UNIX_COMPATIBLE */
}


#endif /* TEST_MAIN_EXECUTOR_H_ */
