#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <math.h>

#include <core/types.h>
#include <dsp/dsp.h>

#define RTEST_BUF_SIZE  0x1000
#define ITERATIONS      10000

namespace native
{
    void lanczos_resample_2x2(float *dst, const float *src, size_t count);
    void lanczos_resample_2x3(float *dst, const float *src, size_t count);
    void lanczos_resample_3x2(float *dst, const float *src, size_t count);
    void lanczos_resample_3x3(float *dst, const float *src, size_t count);
    void lanczos_resample_4x2(float *dst, const float *src, size_t count);
    void lanczos_resample_4x3(float *dst, const float *src, size_t count);
}

namespace sse
{
    void lanczos_resample_2x2(float *dst, const float *src, size_t count);
    void lanczos_resample_2x3(float *dst, const float *src, size_t count);
    void lanczos_resample_3x2(float *dst, const float *src, size_t count);
    void lanczos_resample_3x3(float *dst, const float *src, size_t count);
    void lanczos_resample_4x2(float *dst, const float *src, size_t count);
    void lanczos_resample_4x3(float *dst, const float *src, size_t count);
}

namespace oversampling_test
{
    using namespace lsp;

    void dump_buffer(const char *text, const float *buf, size_t count)
    {
        printf("  dump of buffer %s:\n    ", text);
        while (count--)
            printf("%.5f ", *(buf++));
        printf("\n");
    }

    void call_oversampling(float *out, float *in, size_t count, size_t times, const char *text, resampling_function_t func)
    {
        printf("\nTesting %s resampling on input buffer size %d ...\n", text, int(count));

        // Test 1
        dsp::fill_zero(in, count);
        dsp::fill_zero(out, count*times + 32);
        in[0] = 1.0f;
        func(out, in, count);
        dump_buffer("in", in, count);
        dump_buffer("out", out, count*times);

        // Test 2
        dsp::fill_zero(in, count);
        dsp::fill_zero(out, count*times + 32);
        in[1] = 1.0f;
        func(&out[1], in, count);
        dump_buffer("in", in, count);
        dump_buffer("out", &out[1], count*times);

        // Test 3
        dsp::fill_zero(in, count);
        dsp::fill_zero(out, count*times + 32);
        in[count/2 + 2] = 1.0f;
        func(out, in, count/2 + 3);
        dump_buffer("in", in, count);
        dump_buffer("out", out, count*times);

        // Test 4
        dsp::fill_zero(in, count);
        dsp::fill_zero(out, count*times + 32);
        in[0] = 1.0f;
        in[1] = 1.0f;
        func(out, in, count);
        dump_buffer("in", in, count);
        dump_buffer("out", out, count*times);
    }

    int test(int argc, const char **argv)
    {
        dsp::init();

        float *out          = new float[RTEST_BUF_SIZE*4 + 32];
        float *in           = new float[RTEST_BUF_SIZE];

        call_oversampling(out, in, 16, 2, "2x2 native", native::lanczos_resample_2x2);
        call_oversampling(out, in, 16, 2, "2x2 sse", sse::lanczos_resample_2x2);
        call_oversampling(out, in, 16, 2, "2x3 native", native::lanczos_resample_2x3);
        call_oversampling(out, in, 16, 2, "2x3 sse", sse::lanczos_resample_2x3);
        call_oversampling(out, in, 16, 3, "3x2 native", native::lanczos_resample_3x2);
        call_oversampling(out, in, 16, 3, "3x2 sse", sse::lanczos_resample_3x2);
        call_oversampling(out, in, 16, 3, "3x3 native", native::lanczos_resample_3x3);
        call_oversampling(out, in, 16, 3, "3x3 sse", sse::lanczos_resample_3x3);
        call_oversampling(out, in, 16, 4, "4x2 native", native::lanczos_resample_4x2);
        call_oversampling(out, in, 16, 4, "4x2 sse", sse::lanczos_resample_4x2);
        call_oversampling(out, in, 16, 4, "4x3 native", native::lanczos_resample_4x3);
        call_oversampling(out, in, 16, 4, "4x3 sse", sse::lanczos_resample_4x3);

        for (size_t i=0; i<RTEST_BUF_SIZE; ++i)
        {
            in[i]               = (i % 1) ? 1.0f : -1.0f;
            out[i]              = 0.0f;
        }

        printf("\n\n");

        delete [] out;
        delete [] in;

        return 0;
    }

}

#undef RTEST_BUF_SIZE
#undef ITERATIONS
