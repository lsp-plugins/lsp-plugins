#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <math.h>

#include <core/types.h>
#include <core/dsp.h>

#define ADDM_BUF_SIZE  0x100000

namespace addm_test
{
    using namespace lsp;

    void test_addm(float *a, const float *b, size_t count)
    {
        printf("Testing LOGARITHM of size %d ...\n", int(count));
        clock_t start = clock();
        float time = 0.0f;
        size_t iterations = 0;

        do
        {
            // Do 100 iterations
            for (size_t i=0; i<1000; ++i)
                dsp::add_multiplied(a, b, 0.5f, count);

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

        float *a            = new float[ADDM_BUF_SIZE];
        float *b            = new float[ADDM_BUF_SIZE];

        for (size_t i=0; i<ADDM_BUF_SIZE; ++i)
        {
            a[i]          = 0.0f;
            b[i]          = 1.0f;
        }

        test_addm(a, b, ADDM_BUF_SIZE);

        delete [] a;
        delete [] b;

        return 0;
    }
    
}

#undef ADDM_BUF_SIZE
