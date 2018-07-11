#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <math.h>

#include <core/types.h>
#include <core/dsp.h>

#define FTEST_BUF_SIZE  0x200
#define ITERATIONS      10000

namespace equalizer_test
{
    using namespace lsp;

    void test_equalizer(float *out, const float *in, size_t slope, size_t count)
    {
        printf("Testing Equalizer of 16 channels, slope=%d, size=%d ...\n", int(slope), int(count));

        filter_params_t lopass, hipass;
        lopass.fFreq    = 1000;
        lopass.fGain    = 1.0f;
        lopass.fQuality = 0.707;
        lopass.nSlope   = slope;
        lopass.nType    = FLT_BT_BWC_LOPASS;

        hipass          = lopass;
        hipass.nType    = FLT_BT_BWC_HIPASS;

        Equalizer eq;
        eq.init(16, 13);
        eq.set_sample_rate(48000);
        eq.set_mode(EQM_IIR);

        for (size_t i=0; i<16; i+=2)
        {
            eq.set_params(i*2, &lopass);
            eq.set_params(i*2+1, &lopass);
        }

//        eq.reconfigure();

        // Start
        clock_t start = clock();
        float time = 0.0f;
        size_t iterations = 0;

        do
        {
            // Do 100 iterations
            for (size_t i=0; i<ITERATIONS; ++i)
                eq.process(out, in, count);

            // Calculate statistics
            iterations     += ITERATIONS;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.6f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);

        eq.destroy();
    }

    int test(int argc, const char **argv)
    {
        dsp::init();

        float *out          = new float[FTEST_BUF_SIZE];
        float *in           = new float[FTEST_BUF_SIZE];

        for (size_t i=0; i<FTEST_BUF_SIZE; ++i)
        {
            in[i]               = (i % 1) ? 1.0f : -1.0f;
            out[i]              = 0.0f;
        }

        test_equalizer(out, in, 2, FTEST_BUF_SIZE);
        test_equalizer(out, in, 4, FTEST_BUF_SIZE);
        test_equalizer(out, in, 6, FTEST_BUF_SIZE);
        test_equalizer(out, in, 8, FTEST_BUF_SIZE);

        delete [] out;
        delete [] in;

        return 0;
    }

}

#undef FTEST_BUF_SIZE
#undef ITERATIONS
#undef SUBITERATIONS
