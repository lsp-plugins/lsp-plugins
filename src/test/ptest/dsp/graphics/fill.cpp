/*
 * fill.cpp
 *
 *  Created on: 16 нояб. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <core/sugar.h>

#define MIN_RANK 6
#define MAX_RANK 16

namespace native
{
    void fill_rgba(float *dst, float r, float g, float b, float a, size_t count);
    void fill_hsla(float *dst, float h, float s, float l, float a, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void fill_rgba(float *dst, float r, float g, float b, float a, size_t count);
        void fill_hsla(float *dst, float h, float s, float l, float a, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void fill_rgba(float *dst, float r, float g, float b, float a, size_t count);
        void fill_hsla(float *dst, float h, float s, float l, float a, size_t count);
    }
)

typedef void (* hsla_to_fill_t)(float *dst, float c1, float c2, float c3, float c4, size_t count);

//-----------------------------------------------------------------------------
// Performance test for logarithmic axis calculation
PTEST_BEGIN("dsp.graphics", fill, 5, 5000)

    void call(const char *label, float *dst, size_t count, hsla_to_fill_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s pixels...\n", buf);

        PTEST_LOOP(buf,
            func(dst, 1.0f, 2.0f, 3.0f, 4.0f, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size     = 1 << MAX_RANK;
        uint8_t *data       = NULL;
        float *dst          = alloc_aligned<float>(data, buf_size * 4, 64);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            call("native::fill_rgba", dst, count, native::fill_rgba);
            call("native::fill_hsla", dst, count, native::fill_hsla);

            IF_ARCH_X86(call("sse::fill_rgba", dst, count, sse::fill_rgba));
            IF_ARCH_X86(call("sse::fill_hsla", dst, count, sse::fill_hsla));

            IF_ARCH_ARM(call("neon_d32::fill_rgba", dst, count, neon_d32::fill_rgba));
            IF_ARCH_ARM(call("neon_d32::fill_hsla", dst, count, neon_d32::fill_hsla));

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END



