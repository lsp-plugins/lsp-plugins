/*
 * main.cpp
 *
 *  Created on: 01 сен. 2018 г.
 *      Author: sadko
 */

#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <stdlib.h>
#include <signal.h>

#include <core/types.h>
#include <dsp/dsp.h>
#include <test/ptest.h>
#include <test/utest.h>
#include <test/mtest.h>
#include <metadata/metadata.h>
#include <data/cvector.h>
#include <sys/stat.h>

#ifdef PLATFORM_LINUX
    #include <mcheck.h>
#endif /* PLATFORM_LINUX */

enum test_mode_t
{
    UNKNOWN,
    PTEST,
    UTEST,
    MTEST
};

using namespace lsp;

typedef struct config_t
{
    mode_t                      mode;
    bool                        fork;
    bool                        verbose;
    bool                        debug;
    bool                        list_all;
    bool                        mtrace;
    bool                        ilist;
    size_t                      threads;
    const char                 *outfile;
    const char                 *tracepath;
    cvector<char>               list;
    cvector<char>               ignore;
    cvector<char>               args;
} config_t;

typedef struct stats_t
{
    size_t      total;
    size_t      success;
    double      overall;
    cvector<test::Test> failed; // List of failed tests
    cvector<test::Test> ignored; // List of ignored tests
} stats_t;

typedef struct task_t
{
    pid_t               pid;
    struct timespec     submitted;
    test::UnitTest     *utest;
} task_t;

void out_cpu_info(FILE *out)
{
    dsp::info_t *info;
    info = dsp::info();
    if (info == NULL)
        return;

    fprintf(out, "--------------------------------------------------------------------------------\n");
    fprintf(out, "LSP version: %s\n", LSP_MAIN_VERSION);
    fprintf(out, "--------------------------------------------------------------------------------\n");
    fprintf(out, "CPU information:\n");
    fprintf(out, "  Architecture:   %s\n", info->arch);
    fprintf(out, "  CPU string:     %s\n", info->cpu);
    fprintf(out, "  CPU model:      %s\n", info->model);
    fprintf(out, "  Features:       %s\n", info->features);
    fprintf(out, "--------------------------------------------------------------------------------\n");
    fprintf(out, "\n");
    fflush(out);
    free(info);
}

void start_memcheck(config_t *cfg, test::Test *v)
{
    if (!cfg->mtrace)
        return;

    // Enable memory trace
    char fname[PATH_MAX];
    mkdir(cfg->tracepath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    snprintf(fname, PATH_MAX, "%s/%s.utest.mtrace", cfg->tracepath, v->full_name());
    fname[PATH_MAX-1] = '\0';

    fprintf(stderr, "Enabling memory trace for test '%s' into file '%s'\n", v->full_name(), fname);
    fflush(stderr);

    setenv("MALLOC_TRACE", fname, 1);
    #ifdef PLATFORM_LINUX
        mtrace();
    #endif /* PLATFORM_LINUX */
}

void end_memcheck(config_t *cfg)
{
    // Disable memory trace
    #ifdef PLATFORM_LINUX
    if (cfg->mtrace)
        muntrace();
    #endif /* PLATFORM_LINUX */

    // Validate heap
//    mcheck_check_all();
}

bool match_string(const char *p, const char *m)
{
    while (p != NULL)
    {
        // Get pattern token
        const char *pnext = strchr(p, '.');
        size_t pcount = (pnext == NULL) ? strlen(p) : pnext - p;
        if (pnext != NULL)
            pnext++;

        // Check wildcard
        if ((pcount == 2) && (p[0] == '*') && (p[1] == '*'))
            return true;

        // Get match token
        if ((m == NULL) || (*m == '\0'))
            return false;
        const char *mnext = strchr(m, '.');
        size_t mcount = (mnext == NULL) ? strlen(m) : mnext - m;
        if (mnext != NULL)
            mnext++;

        // Check wildcard
        if ((pcount != 1) || (p[0] != '*'))
        {
            // Check that names match
            if ((pcount != mcount) || (memcmp(p, m, pcount) != 0))
                return false;
        }

        // Move pointers
        p = pnext;
        m = mnext;
    }

    return ((m == NULL) || (*m == '\0'));
}

bool match_list(lsp::cvector<char> &list, test::Test *v, bool match_if_empty)
{
    // Empty list always matches
    if (list.size() <= 0)
        return match_if_empty;

    const char *full = v->full_name();

    for (size_t i=0, n=list.size(); i<n; ++i)
    {
        if (match_string(list.at(i), full))
            return true;
    }

    return false;
}

bool check_test_skip(config_t *cfg, stats_t *stats, test::Test *v)
{
    // Check that test is not ignored
    if (v->ignore())
        return true;

    // Need to check test name and group?
    if (match_list(cfg->ignore, v, false))
    {
        stats->ignored.add(v);
        return true;
    }

    // Is there an explicit list of tests?
    if (!match_list(cfg->list, v, true))
        return true;

    return false;
}

int list_all(const char *text, test::Test *v)
{
    cvector<char> names;

    for ( ; v != NULL; v = v->next_test())
    {
        const char *str = v->full_name();
        if (str != NULL)
            names.add(const_cast<char *>(str));
    }

    size_t n = names.size();
    printf("\n%s (total: %d):\n", text, int(n));

    for (size_t i=0; i<n-1; ++i)
        for (size_t j=i+1; j<n; ++j)
            if (strcmp(names.at(i), names.at(j)) > 0)
                names.swap(i, j);

    for (size_t i=0; i<n; ++i)
    {
        char *name = names.at(i);
        printf("  %s\n", name);
    }
    printf("\n");

    names.flush();
    return 0;
}

int output_stats(stats_t *stats, const char *text)
{
    printf("\n--------------------------------------------------------------------------------\n");
    printf("%s:\n", text);
    printf("  overall time [s]:     %.2f\n", stats->overall);
    printf("  launched:             %d\n", int(stats->total));
    printf("  ignored:              %d\n", int(stats->ignored.size()));
    printf("  succeeded:            %d\n", int(stats->success));
    printf("  failed:               %d\n", int(stats->failed.size()));

    if (stats->failed.size() > 0)
    {
        printf("\n--------------------------------------------------------------------------------\n");
        printf("List of failed tests:\n");
        for (size_t i=0, n=stats->failed.size(); i<n; ++i)
        {
            test::Test *t = stats->failed.at(i);
            printf("  %s\n", t->full_name());
        }
        printf("\n");
        return 2;
    }

    if (stats->ignored.size() > 0)
    {
        printf("\n--------------------------------------------------------------------------------\n");
        printf("List of ignored tests:\n");
        for (size_t i=0, n=stats->ignored.size(); i<n; ++i)
        {
            test::Test *t = stats->ignored.at(i);
            printf("  %s\n", t->full_name());
        }
        printf("\n");
        return 2;
    }

    printf("\n");
    return 0;
}

int execute_ptest(FILE *out, config_t *cfg, test::PerformanceTest *v)
{
    // Execute performance test
    v->set_verbose(cfg->verbose);
    start_memcheck(cfg, v);
    v->execute(cfg->args.size(), const_cast<const char **>(cfg->args.get_array()));
    end_memcheck(cfg);

    // Output peformance test statistics
    printf("\nStatistics of performance test '%s':\n", v->full_name());
    v->dump_stats(stdout);

    if (out != NULL)
    {
        fprintf(out, "--------------------------------------------------------------------------------\n");
        fprintf(out, "Statistics of performance test '%s':\n\n", v->full_name());
        v->dump_stats(out);
        fprintf(out, "\n");
        fflush(out);
    }

    v->free_stats();
    return 0;
}

int execute_mtest(config_t *cfg, test::ManualTest *v)
{
    // Execute performance test
    v->set_verbose(cfg->verbose);
    start_memcheck(cfg, v);
    v->execute(cfg->args.size(), const_cast<const char **>(cfg->args.get_array()));
    end_memcheck(cfg);
    return 0;
}

void utest_sighandler(int signum)
{
    fprintf(stderr, "Unit test time limit exceeded\n");
    exit(2);
}

int execute_utest(config_t *cfg, test::UnitTest *v)
{
    // Set-up timer for deadline expiration
    struct itimerval timer;

    if (!cfg->debug)
    {
        timer.it_interval.tv_sec    = v->time_limit();
        timer.it_interval.tv_usec   = suseconds_t(v->time_limit() * 1e+6) % 1000000L;
        timer.it_value              = timer.it_interval;

        signal(SIGALRM, utest_sighandler);
        if (setitimer(ITIMER_REAL, &timer, NULL) != 0)
        {
            int code = errno;
            fprintf(stderr, "setitimer failed with errno=%d\n", code);
            fflush(stderr);
            exit(4);
        }
    }

    // Execute unit test
    v->set_verbose(cfg->verbose);
    start_memcheck(cfg, v);
    v->execute(cfg->args.size(), const_cast<const char **>(cfg->args.get_array()));
    end_memcheck(cfg);

    // Cancel and disable timer
    if (!cfg->debug)
    {
        timer.it_interval.tv_sec    = 0;
        timer.it_interval.tv_usec   = 0;
        timer.it_value              = timer.it_interval;

        if (setitimer(ITIMER_REAL, &timer, NULL) != 0)
        {
            int code = errno;
            fprintf(stderr, "setitimer failed with errno=%d\n", code);
            fflush(stderr);
            exit(4);
        }
        signal(SIGALRM, SIG_DFL);
    }

    // Return success
    return 0;
}

int launch_ptest(config_t *cfg)
{
    using namespace test;
    PerformanceTest *v = ptest_init();

    // List all tests if requested
    if (v == NULL)
    {
        fprintf(stderr, "No performance tests available\n");
        return -1;
    }
    else if (cfg->list_all)
        return list_all("List of available performance tests", v);

    int result = 0;
    struct timespec ts, start, finish;
    stats_t stats;
    stats.total     = 0;
    stats.success   = 0;
    stats.overall   = 0.0;
    double time     = 0.0;

    clock_gettime(CLOCK_REALTIME, &ts);

    FILE *fd = NULL;
    if (cfg->outfile != NULL)
    {
        fd = fopen(cfg->outfile, "w");
        if (fd == NULL)
        {
            fprintf(stderr, "Could not open output file %s\n", cfg->outfile);
            fflush(stderr);
            return 5;
        }

        out_cpu_info(fd);
    }

    for ( ; v != NULL; v = v->next())
    {
        if (check_test_skip(cfg, &stats, v))
            continue;

        printf("\n--------------------------------------------------------------------------------\n");
        printf("Launching performance test '%s'\n", v->full_name());
        printf("--------------------------------------------------------------------------------\n");

        clock_gettime(CLOCK_REALTIME, &start);

        stats.total     ++;
        if (cfg->fork)
        {
            fflush(stdout);
            fflush(stderr);
            if (fd != NULL)
                fflush(fd);
            pid_t pid = fork();
            if (pid < 0)
            {
                int error = errno;
                fprintf(stderr, "Error while spawning child process %d\n", error);

                result = -2;
                break;
            }
            else if (pid == 0)
                return execute_ptest(fd, cfg, v);
            else
            {
                // Parent process code: wait for nested process execution
                do
                {
                    int w = waitpid(pid, &result, WUNTRACED | WCONTINUED);
                    if (w < 0)
                    {
                        fprintf(stderr, "Waiting for performance test '%s' failed\n", v->full_name());
                        stats.failed.add(v);
                        break;
                    }

                    if (WIFEXITED(result))
                        printf("Performance test '%s' finished, status=%d\n", v->full_name(), WEXITSTATUS(result));
                    else if (WIFSIGNALED(result))
                        printf("Performance test '%s' killed by signal %d\n", v->full_name(), WTERMSIG(result));
                    else if (WIFSTOPPED(result))
                        printf("Performance test '%s' stopped by signal %d\n", v->full_name(), WSTOPSIG(result));
                } while (!WIFEXITED(result) && !WIFSIGNALED(result));
            }
        }
        else
            result = execute_ptest(fd, cfg, v);

        clock_gettime(CLOCK_REALTIME, &finish);
        time = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) * 1e-9;

        printf("Performance test '%s' execution time: %.2f s\n", v->full_name() , time);
        if (result == 0)
            stats.success ++;
        else
            stats.failed.add(v);
    }

    if (fd != NULL)
        fclose(fd);

    clock_gettime(CLOCK_REALTIME, &finish);
    stats.overall = (finish.tv_sec - ts.tv_sec) + (finish.tv_nsec - ts.tv_nsec) * 1e-9;
    return output_stats(&stats, "Overall performance test statistics");
}

int launch_mtest(config_t *cfg)
{
    using namespace test;
    ManualTest *v = mtest_init();
    if (v == NULL)
    {
        fprintf(stderr, "No manual tests available\n");
        return -1;
    }
    else if (cfg->list_all)
        return list_all("List of available manual tests", v);

    int result = 0;
    struct timespec ts, start, finish;
    stats_t stats;
    stats.total     = 0;
    stats.success   = 0;
    stats.overall   = 0.0;
    double time     = 0.0;

    clock_gettime(CLOCK_REALTIME, &ts);

    for ( ; v != NULL; v = v->next())
    {
        // Check that test is not ignored
        if (check_test_skip(cfg, &stats, v))
            continue;

        printf("\n--------------------------------------------------------------------------------\n");
        printf("Launching manual test '%s'\n", v->full_name());
        printf("--------------------------------------------------------------------------------\n");

        clock_gettime(CLOCK_REALTIME, &start);

        stats.total     ++;
        if (cfg->fork)
        {
            fflush(stdout);
            fflush(stderr);
            pid_t pid = fork();
            if (pid < 0)
            {
                int error = errno;
                fprintf(stderr, "Error while spawning child process %d\n", error);

                result = -2;
                break;
            }
            else if (pid == 0)
                return execute_mtest(cfg, v);
            else
            {
                // Parent process code: wait for nested process execution
                do
                {
                    int w = waitpid(pid, &result, WUNTRACED | WCONTINUED);
                    if (w < 0)
                    {
                        fprintf(stderr, "Waiting for manual test '%s' failed\n", v->full_name());
                        stats.failed.add(v);
                        break;
                    }

                    if (WIFEXITED(result))
                        printf("Manual test '%s' finished, status=%d\n", v->full_name(), WEXITSTATUS(result));
                    else if (WIFSIGNALED(result))
                        printf("Manual test '%s' killed by signal %d\n", v->full_name(), WTERMSIG(result));
                    else if (WIFSTOPPED(result))
                        printf("Manual test '%s' stopped by signal %d\n", v->full_name(), WSTOPSIG(result));
                } while (!WIFEXITED(result) && !WIFSIGNALED(result));
            }
        }
        else
            result = execute_mtest(cfg, v);

        clock_gettime(CLOCK_REALTIME, &finish);
        time = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) * 1e-9;

        printf("Manual test '%s' execution time: %.2f s\n", v->full_name(), time);
        if (result == 0)
            stats.success ++;
        else
            stats.failed.add(v);
    }

    clock_gettime(CLOCK_REALTIME, &finish);
    stats.overall = (finish.tv_sec - ts.tv_sec) + (finish.tv_nsec - ts.tv_nsec) * 1e-9;
    return output_stats(&stats, "Overall performance test statistics");
}

bool wait_thread(config_t *cfg, task_t *threads, stats_t *stats)
{
    struct timespec ts;
    int result = 0;
    pid_t pid = -1;
    task_t *t;

    do
    {
        pid = waitpid(-1, &result, WUNTRACED | WCONTINUED);
        if (pid < 0)
        {
            fprintf(stderr, "Waiting for unit test completion failed\n");
            exit(6);
            break;
        }

        // Find the associated unit test process
        t = NULL;
        for (size_t i=0; i<cfg->threads; i++)
            if (threads[i].pid == pid)
                t = &threads[i];
        if (t == NULL)
            return false;

        if (WIFEXITED(result))
            printf("Unit test '%s' finished, status=%d\n", t->utest->full_name(), WEXITSTATUS(result));
        else if (WIFSIGNALED(result))
            printf("Unit test '%s' killed by signal %d\n", t->utest->full_name(), WTERMSIG(result));
        else if (WIFSTOPPED(result))
            printf("Unit test '%s' stopped by signal %d\n", t->utest->full_name(), WSTOPSIG(result));
    } while (!WIFEXITED(result) && !WIFSIGNALED(result));

    // Get time
    clock_gettime(CLOCK_REALTIME, &ts);
    double time = (ts.tv_sec - t->submitted.tv_sec) + (ts.tv_nsec - t->submitted.tv_nsec) * 1e-9;
    printf("Unit test '%s' execution time: %.2f s\n", t->utest->full_name(), time);
    if (result == 0)
        stats->success ++;
    else
        stats->failed.add(t->utest);

    // Free task
    t->pid                  = -1;
    t->submitted.tv_sec     = 0;
    t->submitted.tv_nsec    = 0;
    t->utest                = NULL;

    return true;
}

int wait_threads(config_t *cfg, task_t *threads, stats_t *stats)
{
    // Estimate number of child processes
    size_t nwait = 0;
    for (size_t i=0; i<cfg->threads; i++)
        if (threads[i].pid >= 0)
            nwait++;

    // Wait until all children become completed
    while (nwait > 0)
    {
        if (wait_thread(cfg, threads, stats))
            nwait--;
    }

    return 0;
}

int submit_utest(config_t *cfg, task_t *threads, stats_t *stats, test::UnitTest *v)
{
    if (!cfg->fork)
    {
        struct timespec start, finish;
        clock_gettime(CLOCK_REALTIME, &start);

        int res     = execute_utest(cfg, v);

        clock_gettime(CLOCK_REALTIME, &finish);
        double time = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) * 1e-9;
        printf("Test execution time: %.2f s\n", time);

        if (res == 0)
            stats->success++;
        else
            stats->failed.add(v);
        return 0;
    }

    // Find placeholder
    while (true)
    {
        // Check that we are able to submit task
        task_t *t = NULL;
        for (size_t i=0; i<cfg->threads; i++)
            if (threads[i].pid < 0)
            {
                t = &threads[i];
                break;
            }

        // Is there a placeholder?
        if (t != NULL)
        {
            fflush(stdout);
            fflush(stderr);
            pid_t pid = fork();
            if (pid < 0)
            {
                int error = errno;
                fprintf(stderr, "Error while spawning child process %d\n", error);
                return -2;
            }
            else if (pid == 0)
            {
                int res = execute_utest(cfg, v);
                exit(res);
            }
            else
            {
                clock_gettime(CLOCK_REALTIME, &t->submitted);
                t->pid          = pid;
                t->utest        = v;
                return 0;
            }
        }
        else // Wait for child process completion
            wait_thread(cfg, threads, stats);
    }
}

int launch_utest(config_t *cfg)
{
    using namespace test;
    UnitTest *v = utest_init();

    // List all tests if requested
    if (v == NULL)
    {
        fprintf(stderr, "No unit tests available\n");
        return -1;
    }
    else if (cfg->list_all)
        return list_all("List of available unit tests", v);

    struct timespec start, finish;
    stats_t stats;

    clock_gettime(CLOCK_REALTIME, &start);
    stats.total     = 0;
    stats.success   = 0;

    task_t *threads = (cfg->fork) ? reinterpret_cast<task_t *>(alloca(sizeof(task_t) * cfg->threads)) : NULL;
    if (threads != NULL)
    {
        for (size_t i=0; i<cfg->threads; i++)
        {
            threads[i].pid              = -1;
            threads[i].submitted.tv_sec = 0;
            threads[i].submitted.tv_nsec= 0;
            threads[i].utest            = NULL;
        }
    }

    for ( ; v != NULL; v = v->next())
    {
        // Check that test is not ignored
        if (check_test_skip(cfg, &stats, v))
            continue;

        printf("\n--------------------------------------------------------------------------------\n");
        printf("Launching unit test '%s'\n", v->full_name());
        printf("--------------------------------------------------------------------------------\n");

        stats.total ++;
        if (submit_utest(cfg, threads, &stats, v) != 0)
            break;
    }

    // Wait until all child processes exit
    if (threads != NULL)
    {
        if (wait_threads(cfg, threads, &stats) != 0)
            fprintf(stderr, "Error while waiting child processes for comletion");
    }

    clock_gettime(CLOCK_REALTIME, &finish);
    stats.overall = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) * 1e-9;

    return output_stats(&stats, "Overall unit test statistics");
}

int usage(bool detailed = false)
{
    puts("USAGE: {utest|ptest|mtest} [args...] [test name...]");
    if (!detailed)
        return 1;

    puts("  First argument:");
    puts("    utest                 Unit testing subsystem");
    puts("    ptest                 Performance testing subsystem");
    puts("    mtest                 Manual testing subsystem");
    puts("  Additional arguments:");
    puts("    -a, --args [args...]  Pass arguments to test");
    puts("    -d, --debug           Disable time restrictions for unit tests");
    puts("                          for debugging purporses");
    puts("    -e, --execute         Launch tests specified after this switch");
    puts("    -f, --fork            Fork child processes (opposite to --nofork)");
    puts("    -h, --help            Display help");
    puts("    -i, --ignore          Ignore tests specified after this switch");
    puts("    -j, --jobs            Set number of job workers for unit tests");
    puts("    -l, --list            List all available tests");
    #ifdef PLATFORM_LINUX
    puts("    -mt, --mtrace         Enable mtrace log");
#endif /* PLATFORM_LINUX */
    puts("    -nf, --nofork         Do not fork child processes (for better ");
    puts("                          debugging capabilities)");
#ifdef PLATFORM_LINUX
    puts("    -nt, --nomtrace       Disable mtrace log");
#endif /* PLATFORM_LINUX */
    puts("    -o, --outfile file    Output performance test statistics to specified file");
    puts("    -s, --silent          Do not output additional information from tests");
    puts("    -t, --tracepath path  Override default trace path with specified value");
    puts("    -v, --verbose         Output additional information from tests");
    return 1;
}

int parse_config(config_t *cfg, int argc, const char **argv)
{
    cfg->mode       = UNKNOWN;
    cfg->fork       = true;
    cfg->verbose    = false;
    cfg->list_all   = false;
    cfg->mtrace     = false;
    cfg->ilist      = false;
    cfg->tracepath  = "/tmp/lsp-plugins-trace";
    cfg->threads    = sysconf(_SC_NPROCESSORS_ONLN) * 2;
    cfg->outfile    = NULL;
    if (argc < 2)
        return usage();

    if (!strcmp(argv[1], "ptest"))
        cfg->mode = PTEST;
    else if (!strcmp(argv[1], "utest"))
        cfg->mode = UTEST;
    else if (!strcmp(argv[1], "mtest"))
        cfg->mode = MTEST;
    else if ((!strcmp(argv[1], "--help")) || ((!strcmp(argv[1], "-h"))))
        return usage(true);
    else
        return usage();

    for (int i=2; i<argc; ++i)
    {
        if ((!strcmp(argv[i], "--nofork")) || (!strcmp(argv[i], "-nf")))
            cfg->fork       = false;
        else if ((!strcmp(argv[i], "--fork")) || (!strcmp(argv[i], "-f")))
            cfg->fork       = true;
        else if ((!strcmp(argv[i], "--verbose")) || (!strcmp(argv[i], "-v")))
            cfg->verbose    = true;
        else if ((!strcmp(argv[i], "--silent")) || (!strcmp(argv[i], "-s")))
            cfg->verbose    = false;
        else if ((!strcmp(argv[i], "--debug")) || (!strcmp(argv[i], "-d")))
            cfg->debug      = true;
        else if ((!strcmp(argv[i], "--list")) || (!strcmp(argv[i], "-l")))
            cfg->list_all   = true;
#ifdef PLATFORM_LINUX
        else if ((!strcmp(argv[i], "--mtrace")) || (!strcmp(argv[i], "-mt")))
            cfg->mtrace     = true;
        else if ((!strcmp(argv[i], "--nomtrace")) || (!strcmp(argv[i], "-nt")))
            cfg->mtrace     = false;
#endif /* PLATFORM_LINUX */
        else if ((!strcmp(argv[i], "--tracepath")) || (!strcmp(argv[i], "-t")))
        {
            if ((++i) >= argc)
            {
                fprintf(stderr, "Not specified trace path\n");
                return 3;
            }
            cfg->tracepath  = argv[i];
        }
        else if ((!strcmp(argv[i], "--outfile")) || (!strcmp(argv[i], "-o")))
        {
            if ((++i) >= argc)
            {
                fprintf(stderr, "Not specified name of output file\n");
                return 3;
            }
            cfg->outfile    = argv[i];
        }
        else if ((!strcmp(argv[i], "--args")) || (!strcmp(argv[i], "-a")))
        {
            while (++i < argc)
                cfg->args.add(const_cast<char *>(argv[i]));
        }
        else if ((!strcmp(argv[i], "--jobs")) || (!strcmp(argv[i], "-j")))
        {
            if ((++i) >= argc)
            {
                fprintf(stderr, "Not specified number of jobs for --jobs parameter\n");
                return 3;
            }

            errno           = 0;
            char *end       = NULL;
            cfg->threads    = strtol(argv[i], &end, 10);
            if ((errno != 0) || ((*end) != '\0'))
            {
                fprintf(stderr, "Invalid value for --threads parameter: %s\n", argv[i]);
                return 3;
            }
        }
        else if ((!strcmp(argv[i], "--help")) || ((!strcmp(argv[i], "-h"))))
            return usage(true);
        else if ((!strcmp(argv[i], "--ignore")) || ((!strcmp(argv[i], "-i"))))
            cfg->ilist      = true;
        else if ((!strcmp(argv[i], "--execute")) || ((!strcmp(argv[i], "-e"))))
            cfg->ilist      = false;
        else
        {
            if (cfg->ilist)
                cfg->ignore.add(const_cast<char *>(argv[i]));
            else
                cfg->list.add(const_cast<char *>(argv[i]));
        }
    }

    return 0;
}

void clear_config(config_t *cfg)
{
    cfg->list.flush();
    cfg->ignore.flush();
    cfg->args.flush();
}

int main(int argc, const char **argv)
{
//    // Enable mcheck
//    if (mcheck(NULL) != 0)
//    {
//        fprintf(stderr, "Installing mcheck() failed\n");
//        return -4;
//    }

    config_t cfg;
    int res = parse_config(&cfg, argc, argv);
    if (res != 0)
        return res;

    srand(clock());

    // Nested process code: initialize DSP
    dsp::context_t ctx;
    dsp::init();
    out_cpu_info(stdout);

    dsp::start(&ctx);

    // Launch tests
    switch (cfg.mode)
    {
        case PTEST:
            res = launch_ptest(&cfg);
            break;
        case UTEST:
            res = launch_utest(&cfg);
            break;
        case MTEST:
            res = launch_mtest(&cfg);
            break;
        default:
            break;
    }

    dsp::finish(&ctx);

    clear_config(&cfg);

    return res;
    /*srand(clock());
    lsp_trace("locale is: %s", setlocale(LC_CTYPE, NULL));
    return TEST::test(argc, argv);
    */
}
