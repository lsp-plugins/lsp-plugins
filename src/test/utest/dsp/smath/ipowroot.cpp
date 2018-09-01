/*
 * ipowroot.cpp
 *
 *  Created on: 29 авг. 2018 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <test/helpers.h>
#include <dsp/dsp.h>
#include <math.h>

namespace native
{
    float ipowf(float x, int deg);
    float irootf(float x, int deg);
}

IF_ARCH_X86 (
    namespace sse
    {
        float ipowf(float x, int deg);
        float irootf(float x, int deg);
    }
)

typedef float (* ipowrootf_t)(float x, int deg);

UTEST_BEGIN("dsp.smath", ipowroot)

    void check_native()
    {
        for (int i=-16; i <= 16; ++i)
        {
            float x1 = native::ipowf(M_PI, i);
            float x2 = 1;
            for (size_t j=0, k=(i<0) ? -i: i; j<k; ++j)
                x2 *= M_PI;
            if (i < 0)
                x2 = 1.0f / x2;

            UTEST_ASSERT_MSG(float_equals_relative(x1, x2, 1e-4), "Failed ipowf(PI, %d): x1=%f, x2=%f", int(i), x1, x2);
        }

        for (int i=1; i <= 16; ++i)
        {
            float x1 = native::ipowf(M_PI, i);
            float y = native::irootf(x1, i);

            UTEST_ASSERT_MSG(float_equals_relative(M_PI, y, 1e-4), "Failed irootf(%f, %d) = %f, expected %f", x1, int(i), y, M_PI);
        }
    }

    void call(const char *label, int start, int end, ipowrootf_t func1, ipowrootf_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        printf("Testing %s", label);
        for (int i=start; i <= end; ++i)
        {
            float x1 = func1(M_PI, i);
            float x2 = func2(M_PI, i);

            UTEST_ASSERT_MSG(float_equals_relative(x1, x2, 1e-4), "Failed %s(PI, %d): x1=%f, x2=%f", label, int(i), x1, x2);
        }
    }

    UTEST_MAIN
    {
        check_native();
        IF_ARCH_X86(call("irootf_sse", -16, 16, native::irootf, sse::irootf));
        IF_ARCH_X86(call("ipowf_sse", 1, 16, native::ipowf, sse::ipowf));
    }


UTEST_END



