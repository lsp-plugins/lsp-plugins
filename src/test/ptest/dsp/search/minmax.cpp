/*
 * minmax.cpp
 *
 *  Created on: 08 окт. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>

#define RTEST_BUF_SIZE  0x1000

#define MIN_RANK 8
#define MAX_RANK 20

namespace native
{
    float   min(const float *src, size_t count);
    float   max(const float *src, size_t count);
    void    minmax(const float *src, size_t count, float *min, float *max);

    float   abs_min(const float *src, size_t count);
    float   abs_max(const float *src, size_t count);
    void    abs_minmax(const float *src, size_t count, float *min, float *max);
}

IF_ARCH_X86(
    namespace sse
    {
        float   min(const float *src, size_t count);
        float   max(const float *src, size_t count);
        void    minmax(const float *src, size_t count, float *min, float *max);

        float   abs_min(const float *src, size_t count);
        float   abs_max(const float *src, size_t count);
        void    abs_minmax(const float *src, size_t count, float *min, float *max);
    }
)

typedef float (* search1_t) (const float *src, size_t count);
typedef void  (* search2_t) (const float *src, size_t count, float *min, float *max);

//-----------------------------------------------------------------------------
// Performance test for lanczos resampling
PTEST_BEGIN("dsp.search", minmax, 5, 1000)

    void call(const char *label, const float *in, size_t count, search1_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(in, count);
        );
    }

    void call(const char *label, const float *in, size_t count, search2_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        float min, max;
        PTEST_LOOP(buf,
            func(in, count, &min, &max);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;

        float *in       = alloc_aligned<float>(data, buf_size, 64);
        for (size_t i=0; i < (1 << MAX_RANK); ++i)
            in[i]          = float(rand()) / RAND_MAX;

        for (size_t i=MIN_RANK; i <= MAX_RANK; i += 2)
        {
            size_t count = 1 << i;

            call("native::min", in, count, native::min);
            call("sse::min", in, count, sse::min);

            call("native::abs_min", in, count, native::abs_min);
            call("sse::abs_min", in, count, sse::abs_min);

            call("native::max", in, count, native::max);
            call("sse::max", in, count, sse::max);

            call("native::abs_max", in, count, native::abs_max);
            call("sse::abs_max", in, count, sse::abs_max);

            call("native::minmax", in, count, native::minmax);
            call("sse::minmax", in, count, sse::minmax);

            call("native::abs_minmax", in, count, native::abs_minmax);
            call("sse::abs_minmax", in, count, sse::abs_minmax);

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }

PTEST_END


