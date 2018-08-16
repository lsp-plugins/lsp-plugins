#include <dsp/dsp.h>
#include <test/ptest.h>

#define RTEST_BUF_SIZE  0x1000

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

//-----------------------------------------------------------------------------
// Performance test for lanczos resampling
PTEST_BEGIN("dsp", oversampling, 30, 10000)

    void call(float *out, const float *in, size_t count, size_t times, const char *text, resampling_function_t func)
    {
        printf("Testing %s resampling on input buffer size %d ...\n", text, int(count));

        PTEST_LOOP(
            dsp::fill_zero(out, count*times + 32);
            func(out, in, count);
        );
    }

    void execute()
    {
        float *out          = new float[RTEST_BUF_SIZE*4 + 32];
        float *in           = new float[RTEST_BUF_SIZE];

        for (size_t i=0; i<RTEST_BUF_SIZE; ++i)
            in[i]               = (i % 1) ? 1.0f : -1.0f;
        dsp::fill_zero(out, RTEST_BUF_SIZE * 4 + 32);

        call(out, in, RTEST_BUF_SIZE, 2, "2x2 native", native::lanczos_resample_2x2);
        call(out, in, RTEST_BUF_SIZE, 2, "2x2 sse", sse::lanczos_resample_2x2);
        call(out, in, RTEST_BUF_SIZE, 2, "2x3 native", native::lanczos_resample_2x3);
        call(out, in, RTEST_BUF_SIZE, 2, "2x3 sse", sse::lanczos_resample_2x3);
        call(out, in, RTEST_BUF_SIZE, 3, "3x2 native", native::lanczos_resample_3x2);
        call(out, in, RTEST_BUF_SIZE, 3, "3x2 sse", sse::lanczos_resample_3x2);
        call(out, in, RTEST_BUF_SIZE, 3, "3x3 native", native::lanczos_resample_3x3);
        call(out, in, RTEST_BUF_SIZE, 3, "3x3 sse", sse::lanczos_resample_3x3);
        call(out, in, RTEST_BUF_SIZE, 4, "4x2 native", native::lanczos_resample_4x2);
        call(out, in, RTEST_BUF_SIZE, 4, "4x2 sse", sse::lanczos_resample_4x2);
        call(out, in, RTEST_BUF_SIZE, 4, "4x3 native", native::lanczos_resample_4x3);
        call(out, in, RTEST_BUF_SIZE, 4, "4x3 sse", sse::lanczos_resample_4x3);

        delete [] out;
        delete [] in;
    }

PTEST_END


