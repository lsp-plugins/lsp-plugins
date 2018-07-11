#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <math.h>

#include <core/types.h>
#include <core/dsp.h>

#define SAT_BUF_SIZE  0x100000

namespace saturation_test
{
    using namespace lsp;

    void test_saturation(float *dst, const float *src, size_t count)
    {
        printf("Testing SATURATION of size %d ...\n", int(count));
        clock_t start = clock();
        float time = 0.0f;
        size_t iterations = 0;

        do
        {
            // Do 100 iterations
            for (size_t i=0; i<1000; ++i)
                dsp::copy_saturated(dst, src, count);

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

        float *dst          = new float[SAT_BUF_SIZE];
        float *src          = new float[SAT_BUF_SIZE];

        for (size_t i=0; i<SAT_BUF_SIZE; ++i)
        {
            switch (i%16)
            {
                case 0:
                    src[i]          = +INFINITY;
                    break;
                case 1:
                    src[i]          = -INFINITY;
                    break;
                case 2:
                    src[i]          = NAN;
                    break;
                case 3:
                    src[i]          = -NAN;
                    break;
                default:
                    src[i]          = float(rand()) / RAND_MAX;
                    if (i%2)
                        src[i]          = - src[i];
                    break;
            }
        }

        test_saturation(dst, src, SAT_BUF_SIZE);

        delete [] dst;
        delete [] src;

        return 0;
    }
    
}

#undef SAT_BUF_SIZE
