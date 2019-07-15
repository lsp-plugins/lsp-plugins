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
    void axis_apply_log1(float *x, const float *v, float zero, float norm_x, size_t count);
    void axis_apply_log2(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void axis_apply_log1(float *x, const float *v, float zero, float norm_x, size_t count);
        void axis_apply_log2(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count);
    }

    IF_ARCH_X86_64(
        namespace sse3
        {
            void x64_axis_apply_log1(float *x, const float *v, float zero, float norm_x, size_t count);
            void x64_axis_apply_log2(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count);
        }
    )
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void axis_apply_log1(float *x, const float *v, float zero, float norm_x, size_t count);
        void axis_apply_log2(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count);
    }
)

typedef void (* axis_apply_log1_t)(float *x, const float *v, float zero, float norm_x, size_t count);
typedef void (* axis_apply_log2_t)(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count);

//-----------------------------------------------------------------------------
// Performance test for logarithmic axis calculation
PTEST_BEGIN("dsp.graphics", axis, 5, 1000)

    void call(const char *label, float *x, const float *v, size_t count, axis_apply_log1_t apply)
    {
        if (!PTEST_SUPPORTED(apply))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s points...\n", buf);

        PTEST_LOOP(buf,
            apply(x, v, 1.0f, 1.0f, count);
        );
    }

    void call(const char *label, float *x, float *y, const float *v, size_t count, axis_apply_log2_t apply)
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

            call("native:axis_apply_log1", x, v, count, native::axis_apply_log1);
            IF_ARCH_X86(call("sse:axis_apply_log1", x, v, count, sse::axis_apply_log1));
            IF_ARCH_X86_64(call("sse3:x64_axis_apply_log1", x, v, count, sse3::x64_axis_apply_log1));
            IF_ARCH_ARM(call("neon_d32:axis_apply_log1", x, v, count, neon_d32::axis_apply_log1));

            PTEST_SEPARATOR;

            call("native:axis_apply_log2", x, y, v, count, native::axis_apply_log2);
            IF_ARCH_X86(call("sse:axis_apply_log2", x, y, v, count, sse::axis_apply_log2));
            IF_ARCH_X86_64(call("sse3:x64_axis_apply_log2", x, y, v, count, sse3::x64_axis_apply_log2));
            IF_ARCH_ARM(call("neon_d32:axis_apply_log2", x, y, v, count, neon_d32::axis_apply_log2));

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END


