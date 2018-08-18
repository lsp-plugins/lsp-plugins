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

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

int launch_ptest()
{
    using namespace test::ptest;
    PerformanceTest *v = init();
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
        printf("--------------------------------------------------------------------------------\n");
        printf("Launching performance test '%s' group '%s'\n", v->name(), v->group());
        printf("--------------------------------------------------------------------------------\n");

        clock_gettime(CLOCK_REALTIME, &start);

        pid_t pid = fork();
        if (pid < 0) {
            int error = errno;
            fprintf(stderr, "Error while spawning child process %d\n", error);

            result = -2;
            break;
        } else if (pid == 0) {
            srand(clock());

            // Nested process code: initialize DSP
            dsp::context_t ctx;
            dsp::init();
            dsp::start(&ctx);

            // Execute performance test
            v->execute();

            dsp::finish(&ctx);
            return 0;
        } else {
            // Parent process code: wait for nested process execution
            total ++;
            do
            {
                int w = waitpid(pid, &result, WUNTRACED | WCONTINUED);
                if (w < 0)
                {
                    fprintf(stderr, "Waiting for performance test '%s' group '%s failed\n", v->name(), v->group());
                    failed++;
                    break;
                }

                if (WIFEXITED(result))
                    printf("Performance test '%s' group '%s' finished, status=%d\n", v->name(), v->group(), WEXITSTATUS(result));
                else if (WIFSIGNALED(result))
                    printf("Performance test '%s' group '%s' killed by signal %d\n", v->name(), v->group(), WTERMSIG(result));
                else if (WIFSTOPPED(result))
                    printf("Performance test '%s' group '%s' stopped by signal %d\n", v->name(), v->group(), WSTOPSIG(result));
            } while (!WIFEXITED(result) && !WIFSIGNALED(result));

            clock_gettime(CLOCK_REALTIME, &finish);
            time = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) * 1e-9;

            printf("Test execution time: %.2f s\n", time);
            if (result == 0)
                success ++;
            else
                failed ++;
        }
    }

    time = (finish.tv_sec - ts.tv_sec) + (finish.tv_nsec - ts.tv_nsec) * 1e-9;

    printf("--------------------------------------------------------------------------------\n");
    printf("Overall performance test statistics:\n");
    printf("  overall time [s]:     %.2f\n", time);
    printf("  launched:             %d\n", int(total));
    printf("  succeeded:            %d\n", int(success));
    printf("  failed:               %d\n", int(failed));
    return (failed > 0) ? 0 : 2;
}

enum test_mode_t
{
    UNKNOWN,
    PTEST
};

int usage()
{
    fprintf(stderr, "USAGE: ptest [args...]\n");
    return 1;
}

int main(int argc, const char **argv)
{
    test_mode_t mode = UNKNOWN;
    if (argc < 2)
        return usage();

    if (!strcmp(argv[1], "ptest"))
        mode = PTEST;
    if (mode == UNKNOWN)
        return usage();

    // Launch tests
    int res = 0;
    switch (mode)
    {
        case PTEST:
            res = launch_ptest();
            break;
        default:
            break;
    }

    return res;
    /*srand(clock());
    lsp_trace("locale is: %s", setlocale(LC_CTYPE, NULL));
    return TEST::test(argc, argv);
    */
}
