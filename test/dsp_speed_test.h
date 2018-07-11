#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>

#include <core/types.h>
#include <core/dsp.h>

#define BUF_SIZE            (1024 * 1024 * 32)
#define ITERATIONS          100
#define ALIGN_PAD           64
#define FLOAT_PAD           (ALIGN_PAD / sizeof(float))

namespace dsp_speed_test
{
    using namespace lsp;

    template <class T>
        inline T *align(T *ptr)
        {
            ptrdiff_t addr  = ptrdiff_t(ptr);
            ptrdiff_t offset= addr % ALIGN_PAD;
            if (offset > 0)
                addr           += ALIGN_PAD - offset;
            return reinterpret_cast<T *>(addr);
        }

    void test_function(float *dst, const float *src1, const float *src2)
    {
        printf("Testing dst=%p, src1=%p, src2=%p...\n", dst, src1, src2);
        clock_t start = clock();
        float time = 0.0f;
        size_t iterations = 0;

        do
        {
            // Do 100 iterations
            for (size_t i=0; i<ITERATIONS; ++i)
                dsp::multiply(dst, src1, src2, BUF_SIZE);

            // Calculate statistics
            iterations     += ITERATIONS;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.5f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);
    }

    int test(int argc, const char **argv)
    {
        dsp::init();

        float *src1         = new float[BUF_SIZE + FLOAT_PAD];
        float *src2         = new float[BUF_SIZE + FLOAT_PAD];
        float *dst          = new float[BUF_SIZE + FLOAT_PAD];

        float *s1           = align(src1);
        float *s2           = align(src2);
        float *d            = align(dst);

        for (size_t i=0; i < BUF_SIZE; ++i)
        {
            s1[i]               = float(rand()) / RAND_MAX;
            s2[i]               = float(rand()) / RAND_MAX;
        }

        test_function(d, s1, s2);

        delete [] src1;
        delete [] src2;
        delete [] dst;

        return 0;
    }
    
}

#undef ITERATIONS
