#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>

#include <core/types.h>
#include <core/dsp.h>
#include <core/Randomizer.h>

#define BUF_ROWS    32

namespace randgen_test
{
    using namespace lsp;

    int test(int argc, const char **argv)
    {
        dsp::init();

        Randomizer rnd;
        rnd.init();

        int *counters   = new int[BUF_ROWS * BUF_ROWS];
        for (size_t i=0; i < BUF_ROWS*BUF_ROWS; ++i)
            counters[i]     = 0;

        for (size_t i=0; i<(BUF_ROWS*BUF_ROWS*1024); ++i)
        {
            size_t idx  = BUF_ROWS * BUF_ROWS * rnd.random(RND_TRIANGLE);
            counters[idx]++;
        }

        float max = 0;
        for (size_t i=0; i < BUF_ROWS*BUF_ROWS; ++i)
            if (max < counters[i])
                max = counters[i];
        max = 1.0f / max;

        for (size_t i=0; i<BUF_ROWS; ++i)
        {
            for (size_t j=0; j<BUF_ROWS; ++j)
                printf("%.3f ", counters[j * BUF_ROWS + i] * max);
            printf("\n");
        }

        printf("Probabilities:\n");
        printf("id;value\n");
        for (size_t i=0; i<BUF_ROWS * BUF_ROWS; ++i)
            printf("%d;%.3f\n", int(i), counters[i] * max);

        delete [] counters;

        printf("Random noise:\n");
        for (size_t i=0; i<1024; ++i)
            printf("%d;%.5f\n", int(i), rnd.random(RND_TRIANGLE) - 0.5f);

        return 0;
    }
    
}

#undef BUF_ROWS

