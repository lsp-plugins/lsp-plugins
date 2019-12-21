/*
 * latency_detector.cpp
 *
 *  Created on: 27 авг. 2018 г.
 *      Author: sadko
 */

#include <test/mtest.h>
#include <test/helpers.h>

#include <core/types.h>
#include <core/alloc.h>
#include <data/cvector.h>

#include <core/envelope.h>
#include <math.h>
#include <core/util/LatencyDetector.h>
#include <stdlib.h>
#include <time.h>

#include <dsp/dsp.h>

#define LATENCYDETECTOR_BUFFER_SIZE 1024
#define LATENCYDETECTOR_SAMPLE_RATE 96000
#define LATENCYDETECTOR_MAX_DELAY   LATENCYDETECTOR_BUFFER_SIZE - 1
#define LATENCYDETECTOR_DELAY_STEP  1

using namespace lsp;

MTEST_BEGIN("core.util", latency_detector)

    void test_latencydetector(float *out, float *bak, float *in, size_t *delay_exact, size_t *delay_detected, size_t buf_count, size_t bak_count)
    {
        printf("Testing LatencyDetector... ");

        // Initialise the LatencyDetector object:
        LatencyDetector ld;
        ld.init();
        ld.set_sample_rate(LATENCYDETECTOR_SAMPLE_RATE);
        ld.set_delay_ratio(0.5f);
        ld.set_peak_threshold(0.5f);
        ld.set_abs_threshold(0.01f);

        // Test Parameters
    //        ld.set_duration(0.015f);
    //        ld.set_fading(0.030f);
    //        ld.set_pause(0.025f);

        // Random Parameters:
        ld.set_duration((float(rand()) * 0.040f / RAND_MAX) + 0.010f);
        ld.set_op_fading((float(rand()) * 0.045f / RAND_MAX) + 0.005f);
        ld.set_op_pause((float(rand()) * 0.045f / RAND_MAX) + 0.005f);

        ld.set_ip_detection((3 * 0.050f) + (float(LATENCYDETECTOR_MAX_DELAY) / LATENCYDETECTOR_SAMPLE_RATE) + 0.005f);

        if (ld.needs_update())
            ld.update_settings();

        ld.start_capture();

    //        size_t bak_head = 0;

        while (true)
        {
    //            dump_buffer("in", in, buf_count);
    //            dump_buffer("out", out, buf_count);
            ld.process(out, in, buf_count);

            if (ld.cycle_complete())
                break;

            // This will work only for delays shorter than the buffer length

            // When in CS_DETECT, in out there will be the chirp. Let's put a
            // delayed copy into in.

            // Old tail of out is in the beginning:
            dsp::copy(in, bak, *delay_exact);

            // Body of the delayed signal after:
            dsp::copy(&in[*delay_exact], out, buf_count - *delay_exact);

            // Save out tail:
            dsp::copy(bak, &out[buf_count - *delay_exact], *delay_exact);

            // Do Scaling (opposite phase with negative sign).
            dsp::mul_k2(in, -1.0f, buf_count);
        }

        // Now latency is detected:
        *delay_detected = ld.get_latency_samples() - buf_count;

        if (*delay_exact == *delay_detected)
        {
            printf("Success\n");
        }
        else
        {
            printf("FAIL: %lu", (long unsigned)(*delay_exact));
            printf(" VS %lu\n", (long unsigned)(*delay_detected));
        }
    }

    MTEST_MAIN
    {
        size_t buf_size         = LATENCYDETECTOR_BUFFER_SIZE;
        size_t bak_size         = LATENCYDETECTOR_BUFFER_SIZE * ceil(float(LATENCYDETECTOR_MAX_DELAY) / LATENCYDETECTOR_BUFFER_SIZE);

        float *out              = new float[buf_size];
        float *in               = new float[buf_size];
        float *bak              = new float[bak_size];

        size_t delay_array_size     = ceil(float(LATENCYDETECTOR_MAX_DELAY) / LATENCYDETECTOR_DELAY_STEP);
        size_t *delays_exact        = new size_t[delay_array_size];
        size_t *delays_detected     = new size_t[delay_array_size];
        ssize_t *delays_error       = new ssize_t[delay_array_size];

        size_t delay_idx = 0;

        size_t count_success    = 0;
        size_t count_fail       = 0;
        size_t count_total      = 0;

        for (size_t delay = 0; delay < LATENCYDETECTOR_MAX_DELAY; delay += LATENCYDETECTOR_DELAY_STEP)
        {
            printf("%.1f Percent:\t", 100.0f * float(delay) / (LATENCYDETECTOR_MAX_DELAY - 1));

            delays_exact[delay_idx] = delay;
            dsp::fill_zero(out, buf_size);
            dsp::fill_zero(in, buf_size);
            dsp::fill_zero(bak, bak_size);

            test_latencydetector(out, bak, in, &delays_exact[delay_idx], &delays_detected[delay_idx], buf_size, bak_size);

            count_total += 1;
            if (delays_exact[delay_idx] == delays_detected[delay_idx])
                count_success   ++;
            else
                count_fail      ++;

            delays_error[delay_idx] = delays_exact[delay_idx] - delays_detected[delay_idx];

            delay_idx += 1;
        }

//        dump_buffer("exact_latency",    delays_exact,       delay_array_size);
//        dump_buffer("detected_latency", delays_detected,    delay_array_size);
        dump_buffer("delays_error", delays_error,    delay_array_size);
        printf("Success Rate:\t%.1f percent\n", 100 * float(count_success) / count_total);
        printf("Fail Rate:\t%.1f percent\n", 100 * float(count_fail) / count_total);

        delete [] out;
        delete [] in;
        delete [] bak;
        delete [] delays_exact;
        delete [] delays_detected;
    }

MTEST_END





