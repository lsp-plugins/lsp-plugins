/*
 * axis.cpp
 *
 *  Created on: 23 авг. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <core/sugar.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace native
{
    void axis_apply_log(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void axis_apply_log(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count);
    }

    IF_ARCH_X86_64(
        namespace sse3
        {
            void x64_axis_apply_log(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count);
        }
    )
)

typedef void (* axis_apply_log_t)(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count);

//-----------------------------------------------------------------------------
// Performance test for logarithmic axis calculation
PTEST_BEGIN("dsp.graphics", axis_apply_log, 5, 10000)

    void call(const char *label, float *x, float *y, const float *v, size_t count, axis_apply_log_t apply)
    {
        if (!PTEST_SUPPORTED(apply))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s points...\n", buf);

        PTEST_LOOP(buf,
            apply(x, y, v, 1.0f, 1.0f, 1.0f, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size     = 1 << MAX_RANK;
        uint8_t *data       = NULL;
        float *ptr          = alloc_aligned<float>(data, buf_size * 3, 64);

        float *x            = ptr;
        float *y            = &x[buf_size];
        float *v            = &y[buf_size];

        for (size_t i=0; i<buf_size; ++i)
             v[i]          = (float(rand()) / RAND_MAX) + 0.0f;

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            call("native", x, y, v, count, native::axis_apply_log);
            IF_ARCH_X86(call("sse", x, y, v, count, sse::axis_apply_log));
            IF_ARCH_X86_64(call("x64_sse3", x, y, v, count, sse3::x64_axis_apply_log));

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END


