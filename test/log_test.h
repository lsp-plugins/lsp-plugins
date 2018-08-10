#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <math.h>

#include <core/types.h>
#include <dsp/dsp.h>

#define LOG_BUF_SIZE  0x100000

namespace log_test
{
    using namespace lsp;

    void test_logarithm(float *x, float *y, const float *v, size_t count)
    {
        printf("Testing LOGARITHM of size %d ...\n", int(count));
        clock_t start = clock();
        float time = 0.0f;
        size_t iterations = 0;

        do
        {
            // Do 100 iterations
            for (size_t i=0; i<1000; ++i)
                dsp::axis_apply_log(x, y, v, 1.0, 2.0, 3.0, count);

            // Calculate statistics
            iterations     += 1000;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.5f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);
    }

    int test(int argc, const char **argv)
    {
        dsp::init();

        float *x            = new float[LOG_BUF_SIZE];
        float *y            = new float[LOG_BUF_SIZE];
        float *v            = new float[LOG_BUF_SIZE];

        for (size_t i=0; i<LOG_BUF_SIZE; ++i)
            v[i]          = float(rand()) / RAND_MAX;

        v[0]    = 1.0f;
        v[1]    = 0.1f;
        v[2]    = 10.0f;
        v[3]    = M_E;

        test_logarithm(x, y, v, LOG_BUF_SIZE);

        delete [] x;
        delete [] y;
        delete [] v;

        return 0;
    }
    
}

#undef LOG_BUF_SIZE
