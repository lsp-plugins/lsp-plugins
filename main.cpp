#include "test/common.h"

#include "test/ladspa_test.h"
#include "test/vst_test.h"
#include "test/res_gen.h"
#include "test/audiofile_test.h"
#include "test/convolver_test.h"
#include "test/dsp_speed_test.h"
#include "test/window_test.h"
#include "test/env_test.h"
#include "test/frac_test.h"
#include "test/sample_player_test.h"
#include "test/randgen_test.h"
#include "test/fade_test.h"
#include "test/resample_test.h"
#include "test/genttl.h"
#include "test/lv2_buf_size.h"
#include "test/alloc_test.h"
#include "test/jack_test.h"
#include "test/profiling_test.h"

#include "test/fft_test.h"
#include "test/fft_test2.h"
#include "test/fft_test3.h"
#include "test/fft_speed_test.h"
#include "test/fft_conv_test.h"
#include "test/fft_fastconv_test.h"
#include "test/fft_fastconv_speed_test.h"
#include "test/fft_tdomain_test.h"
#include "test/fft_filter.h"
#include "test/fft_join.h"

#include "test/complex_mul_speed.h"
#include "test/real_to_complex.h"

#include "test/crossover_test.h"
#include "test/saturation_test.h"
#include "test/log_test.h"
#include "test/addm_test.h"
#include "test/perf/filter_test.h"
#include "test/perf/dyn_filter_test.h"
#include "test/perf/bt_perf_test.h"
#include "test/perf/mt_perf_test.h"
#include "test/equalizer_test.h"
#include "test/algo/avx_test.h"
#include "test/sidechain_test.h"
#include "test/limiter_test.h"

#include "test/files/config_test.h"
#include "test/files/text_read_test.h"

#include "test/sse_test.h"
#include "test/endian_test.h"

#include "test/oversampling_test.h"
#include "test/downsampling_test.h"
#include "test/oversampler_test.h"

//#include "test/geometry3d_test.h"
//#include "test/objfile3d_test.h"
//#include "test/anyfile3d_test.h"

#include "test/latencydetector_test.h"
#include "test/lspstring.h"
#include "test/lspc_test.h"
#include "test/clipbrd_test.h"
#include "test/solutions3d_test.h"
#include "test/synchronizedchirp_test.h"

#include "test/algo/bt_test.h"
#include "test/algo/mt_test.h"

#include "test/math/root_test.h"

#include "test/x11/selection_test.h"


//#define TEST config_test
//#define TEST text_read_test

//#define TEST xwindow_test
//#define TEST limiter_test
//#define TEST fft_conv_test
//#define TEST crossover_test
//#define TEST saturation_test
//#define TEST log_test
//#define TEST addm_test
//#define TEST filter_test
//#define TEST dyn_filter_test
//#define TEST equalizer_test
//#define TEST avx_test
//#define TEST sidechain_test

//#define TEST ladspa_test
//#define TEST vst_test
//#define TEST res_gen
//#define TEST audiofile_test
//#define TEST convolver_test

//#define TEST fft_test
//#define TEST fft_test2
//#define TEST fft_test3
//#define TEST fft_tdomain_test
//#define TEST fft_fastconv_test
//#define TEST fft_fastconv_speed_test
//#define TEST fft_filter_test
//#define TEST fft_speed_test
//#define TEST fft_join_test

#define TEST complex_mul_speed_test
//#define TEST real_to_complex_test

//#define TEST dsp_speed_test
//#define TEST window_test
//#define TEST env_test
//#define TEST sample_player_test
//#define TEST frac_test
//#define TEST randgen_test
//#define TEST fade_test
//#define TEST resample_test
//#define TEST genttl_test
//#define TEST lv2bufsize_test
//#define TEST alloc_test
//#define TEST profiling_test
//#define TEST lspstring_test
//#define TEST clipbrd_test

//#define TEST downsampling_test
//#define TEST oversampling_test
//#define TEST oversampler_test

//#define TEST sse_test
//#define TEST latencydetector_test
//#define TEST geometry3d_test
//#define TEST solutions3d_test
//#define TEST objfile3d_test
//#define TEST anyfile3d_test
//#define TEST synchronizedchirp_test
//#define TEST lspc_test

//#define TEST bt_test
//#define TEST mt_test
//#define TEST bt_perf_test
//#define TEST mt_perf_test
//#define TEST root_test

//#define TEST jack_test
//#define TEST selection_test
//#define TEST endian_test

#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <data/cvector.h>
#include <core/LSPString.h>

enum test_mode_t
{
    UNKNOWN,
    PTEST
};

using namespace lsp;

typedef struct config_t
{
    mode_t                      mode;
    bool                        fork;
    bool                        verbose;
    cvector<LSPString>          list;
} config_t;

bool match_string(const LSPString *p, const LSPString *m)
{
    size_t m_off = 0;

    for (size_t off = 0, len = p->length(); off < len; )
    {
        // Get pattern token
        LSPString *pt = NULL;
        ssize_t next = p->index_of(off, '.');
        if (next < 0)
        {
            pt = p->substring(off);
            off = p->length();
        }
        else
        {
            pt = p->substring(off, next);
            off = next + 1;
        }

        // Get match token
        if (m_off >= m->length())
            return false;

        LSPString *mt = NULL;
        next = m->index_of(m_off, '.');
        if (next < 0)
        {
            mt = m->substring(m_off);
            m_off = m->length();
        }
        else
        {
            mt = m->substring(m_off, next);
            m_off = next + 1;
        }

        // Check wildcard
        if ((pt->length() == 1) && (pt->char_at(0) == '*'))
            continue;

        // Check equality
        if (!pt->equals(mt))
            return false;
    }

    return true;
}

bool match_list(lsp::cvector<LSPString> &list, const char *group, const char *name)
{
    // Empty list always matches
    if (list.size() <= 0)
        return true;

    LSPString m;
    m.set_ascii(group);
    m.append('.');
    m.append_ascii(name);

    for (size_t i=0, n=list.size(); i<n; ++i)
    {
        LSPString *p = list.at(i);
        if (p == NULL)
            continue;
        if (match_string(p, &m))
            return true;
    }

    return false;
}

int execute_ptest(config_t *cfg, test::PerformanceTest *v)
{
    // Execute performance test
    v->set_verbose(cfg->verbose);
    v->execute();

    // Output peformance test statistics
    printf("\nStatistics of performance test '%s.%s':\n", v->group(), v->name());
    v->dump_stats();
    v->free_stats();
    return 0;
}

int launch_ptest(config_t *cfg)
{
    using namespace test;
    PerformanceTest *v = ptest_init();
    if (v == NULL)
    {
        fprintf(stderr, "No performance tests available\n");
        return -1;
    }

    int result = 0;
    struct timespec ts, start, finish;
    size_t total = 0, success = 0, failed = 0;
    double time = 0.0;

    clock_gettime(CLOCK_REALTIME, &ts);

    for ( ; v != NULL; v = v->next())
    {
        // Check that test is not ignored
        if (v->ignore())
            continue;

        // Need to check test name and group?
        if (!match_list(cfg->list, v->group(), v->name()))
            continue;

        printf("\n--------------------------------------------------------------------------------\n");
        printf("Launching performance test '%s.%s'\n", v->group(), v->name());
        printf("--------------------------------------------------------------------------------\n");

        clock_gettime(CLOCK_REALTIME, &start);

        if (cfg->fork)
        {
            pid_t pid = fork();
            if (pid < 0) {
                int error = errno;
                fprintf(stderr, "Error while spawning child process %d\n", error);

                result = -2;
                break;
            } else if (pid == 0) {
                return execute_ptest(cfg, v);
            } else {
                // Parent process code: wait for nested process execution
                total ++;
                do
                {
                    int w = waitpid(pid, &result, WUNTRACED | WCONTINUED);
                    if (w < 0)
                    {
                        fprintf(stderr, "Waiting for performance test '%s.%s' failed\n", v->group(), v->name());
                        failed++;
                        break;
                    }

                    if (WIFEXITED(result))
                        printf("Performance test '%s.%s' finished, status=%d\n", v->group(), v->name(), WEXITSTATUS(result));
                    else if (WIFSIGNALED(result))
                        printf("Performance test '%s.%s' killed by signal %d\n", v->group(), v->name(), WTERMSIG(result));
                    else if (WIFSTOPPED(result))
                        printf("Performance test '%s.%s' stopped by signal %d\n", v->group(), v->name(), WSTOPSIG(result));
                } while (!WIFEXITED(result) && !WIFSIGNALED(result));
            }
        }
        else
        {
            total ++;
            result = execute_ptest(cfg, v);
        }
        clock_gettime(CLOCK_REALTIME, &finish);
        time = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) * 1e-9;

        printf("Test execution time: %.2f s\n", time);
        if (result == 0)
            success ++;
        else
            failed ++;
    }

    time = (finish.tv_sec - ts.tv_sec) + (finish.tv_nsec - ts.tv_nsec) * 1e-9;

    printf("\n--------------------------------------------------------------------------------\n");
    printf("Overall performance test statistics:\n");
    printf("  overall time [s]:     %.2f\n", time);
    printf("  launched:             %d\n", int(total));
    printf("  succeeded:            %d\n", int(success));
    printf("  failed:               %d\n", int(failed));
    return (failed > 0) ? 0 : 2;
}

int usage()
{
    fprintf(stderr, "USAGE: ptest [args...]\n");
    return 1;
}

int parse_config(config_t *cfg, int argc, const char **argv)
{
    cfg->mode       = UNKNOWN;
    cfg->fork       = true;
    cfg->verbose    = false;
    if (argc < 2)
        return usage();

    if (!strcmp(argv[1], "ptest"))
        cfg->mode = PTEST;
    if (cfg->mode == UNKNOWN)
        return usage();

    for (int i=2; i<argc; ++i)
    {
        if (!strcmp(argv[i], "--nofork"))
            cfg->fork       = false;
        else if (!strcmp(argv[i], "--fork"))
            cfg->fork       = true;
        else if (!strcmp(argv[i], "--verbose"))
            cfg->verbose    = true;
        else if (!strcmp(argv[i], "--silent"))
            cfg->verbose    = false;
        else
        {
            LSPString *s = new LSPString();
            s->set_native(argv[i]);
            cfg->list.add(s);
        }
    }

    return 0;
}

void clear_config(config_t *cfg)
{
    for (size_t i=0, n=cfg->list.size(); i<n; ++i)
        delete cfg->list.at(i);
    cfg->list.clear();
}

int main(int argc, const char **argv)
{
    config_t cfg;
    int res = parse_config(&cfg, argc, argv);
    if (res != 0)
        return res;

    srand(clock());

    // Nested process code: initialize DSP
    dsp::context_t ctx;
    dsp::info_t *info;

    dsp::init();
    info = dsp::info();
    if (info != NULL)
    {
        printf("--------------------------------------------------------------------------------\n");
        printf("CPU information:\n");
        printf("  Architecture:   %s\n", info->arch);
        printf("  CPU string:     %s\n", info->cpu);
        printf("  CPU model:      %s\n", info->model);
        printf("  Features:       %s\n", info->features);
        free(info);
    }

    dsp::start(&ctx);

    // Launch tests
    switch (cfg.mode)
    {
        case PTEST:
            res = launch_ptest(&cfg);
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
