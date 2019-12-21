/*
 * process.cpp
 *
 *  Created on: 13 авг. 2019 г.
 *      Author: sadko
 */

#include <core/alloc.h>
#include <test/utest.h>
#include <core/ipc/Process.h>
#include <core/ipc/Thread.h>
#include <core/system.h>

using namespace lsp;

#define ENV_VAR_NAME        "LSP_TEST_ENV_VAR"

UTEST_BEGIN("core.ipc", process)
    UTEST_TIMELIMIT(10)

    void test_envs(ipc::Process &p)
    {
        size_t envs = p.envs();
        UTEST_ASSERT(envs > 0);
        LSPString key, value;

        for (size_t i=0; i<envs; ++i)
        {
            UTEST_ASSERT(p.read_env(i, &key, &value) == STATUS_OK);
            printf("  env: %s = %s\n", key.get_native(), value.get_native());
        }

        UTEST_ASSERT(p.get_env(ENV_VAR_NAME, &value) == STATUS_NOT_FOUND);
        UTEST_ASSERT(p.set_env(ENV_VAR_NAME, "123456") == STATUS_OK);
        UTEST_ASSERT(envs == (p.envs() - 1));
        value.clear();
        UTEST_ASSERT(p.get_env(ENV_VAR_NAME, &value) == STATUS_OK);
        printf("get_env(%s) = %s\n", ENV_VAR_NAME, value.get_native());
        UTEST_ASSERT(value.equals_ascii("123456"));
        value.clear();
        UTEST_ASSERT(p.remove_env(ENV_VAR_NAME, &value) == STATUS_OK);
        UTEST_ASSERT(value.equals_ascii("123456"));
        UTEST_ASSERT(envs == p.envs());
        UTEST_ASSERT(p.clear_env() == STATUS_OK);
        UTEST_ASSERT(p.envs() == 0);
        UTEST_ASSERT(p.copy_env() == STATUS_OK);
        UTEST_ASSERT(p.envs() == envs);
    }

    void test_args(ipc::Process &p)
    {
        UTEST_ASSERT(p.args() == 0);
        LSPString tmp;
        char *ctmp = NULL;

        UTEST_ASSERT(p.add_arg("arg1") == STATUS_OK);
        UTEST_ASSERT(p.add_arg("arg4") == STATUS_OK);
        UTEST_ASSERT(tmp.set_ascii("arg3"));
        UTEST_ASSERT(p.add_arg(&tmp) == STATUS_OK);
        UTEST_ASSERT(p.insert_arg(1, "arg2") == STATUS_OK);
        UTEST_ASSERT(p.args() == 4);
        UTEST_ASSERT(p.remove_arg(2, &tmp) == STATUS_OK);
        UTEST_ASSERT(tmp.equals_ascii("arg4"));
        UTEST_ASSERT(p.args() == 3);

        UTEST_ASSERT(p.get_arg(0, &ctmp) == STATUS_OK);
        UTEST_ASSERT(strcmp(ctmp, "arg1") == 0);
        ::free(ctmp);

        UTEST_ASSERT(p.get_arg(1, &ctmp) == STATUS_OK);
        UTEST_ASSERT(strcmp(ctmp, "arg2") == 0);
        ::free(ctmp);

        tmp.clear();
        UTEST_ASSERT(p.get_arg(2, &tmp) == STATUS_OK);
        UTEST_ASSERT(tmp.equals_ascii("arg3"));

        UTEST_ASSERT(p.clear_args() == STATUS_OK);
        UTEST_ASSERT(p.args() == 0);
    }

    UTEST_MAIN
    {
        if (argc <= 0)
        {
            ipc::Process p;
            test_envs(p);
            test_args(p);

            LSPString cmd;
#ifdef PLATFORM_WINDOWS
            UTEST_ASSERT(cmd.set_utf8(executable()));
#else
            UTEST_ASSERT(cmd.set_native(executable()));
#endif

            // Form the command line
            p.set_command(&cmd);
            p.add_arg("utest");
            p.add_arg("--debug");
            p.add_arg("--verbose");
            p.add_arg("--nofork");
            p.add_arg(full_name());
            p.add_arg("--args");
            p.add_arg("child");
            p.add_arg("arg1");
            p.add_arg("arg2");

            // Set environment
            p.set_env(ENV_VAR_NAME, "test_value");

            // Redirect stdout and stderr
            io::IInStream *xstdout = p.get_stdout();
            UTEST_ASSERT(xstdout != NULL);
            io::IInStream *xstderr = p.get_stderr();
            UTEST_ASSERT(xstderr != NULL);

            // Launch the process
            printf("Starting child process...\n");
            UTEST_ASSERT(p.process_id() < 0);
            UTEST_ASSERT(p.launch() == STATUS_OK);
            printf("Started child process, pid=%d\n", int(p.process_id()));
            UTEST_ASSERT(p.wait(0) == STATUS_OK); // Test process status
            UTEST_ASSERT(p.wait(200) == STATUS_OK); // Wait for a short while
            UTEST_ASSERT(p.status() == ipc::Process::PSTATUS_RUNNING); // Check status
            printf("Waiting for process...\n");
            UTEST_ASSERT(p.wait() == STATUS_OK); // Wait until termination

            // Close stdout and stderr
            UTEST_ASSERT(xstdout->close() == STATUS_OK);
            UTEST_ASSERT(xstderr->close() == STATUS_OK);

            // Analyze exit status
            int code = 0;
            UTEST_ASSERT(p.exit_code(&code) == STATUS_OK);
            printf("Waited process has terminated with code=%d\n", int(code));
            UTEST_ASSERT(code == 0);

            printf("Parent process has exited\n");
        }
        else
        {
            printf("Child process started\n");

            // Perform argument check
            UTEST_ASSERT(argc == 3);
            UTEST_ASSERT(strcmp(argv[0], "child") == 0);
            UTEST_ASSERT(strcmp(argv[1], "arg1") == 0);
            UTEST_ASSERT(strcmp(argv[2], "arg2") == 0);

            LSPString value;
            UTEST_ASSERT(system::get_env_var(ENV_VAR_NAME, &value) == STATUS_OK);
            UTEST_ASSERT(value.equals_ascii("test_value"));

            // Perform a sleep for synchronization with caller
            printf("Entering sleep\n");
            ipc::Thread::sleep(2 * 1000);
            printf("Child process has exited\n");
        }
    }
UTEST_END;


