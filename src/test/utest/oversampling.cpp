/*
 * oversampling.cpp
 *
 *  Created on: 21 авг. 2018 г.
 *      Author: vsadovnikov
 */

#include <dsp/dsp.h>
#include <test/utest.h>

namespace native
{
    void lanczos_resample_2x2(float *dst, const float *src, size_t count);
    void lanczos_resample_2x3(float *dst, const float *src, size_t count);
    void lanczos_resample_3x2(float *dst, const float *src, size_t count);
    void lanczos_resample_3x3(float *dst, const float *src, size_t count);
    void lanczos_resample_4x2(float *dst, const float *src, size_t count);
    void lanczos_resample_4x3(float *dst, const float *src, size_t count);
    void lanczos_resample_6x2(float *dst, const float *src, size_t count);
    void lanczos_resample_6x3(float *dst, const float *src, size_t count);
    void lanczos_resample_8x2(float *dst, const float *src, size_t count);
    void lanczos_resample_8x3(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void lanczos_resample_2x2(float *dst, const float *src, size_t count);
        void lanczos_resample_2x3(float *dst, const float *src, size_t count);
        void lanczos_resample_3x2(float *dst, const float *src, size_t count);
        void lanczos_resample_3x3(float *dst, const float *src, size_t count);
        void lanczos_resample_4x2(float *dst, const float *src, size_t count);
        void lanczos_resample_4x3(float *dst, const float *src, size_t count);
        void lanczos_resample_6x2(float *dst, const float *src, size_t count);
        void lanczos_resample_6x3(float *dst, const float *src, size_t count);
        void lanczos_resample_8x2(float *dst, const float *src, size_t count);
        void lanczos_resample_8x3(float *dst, const float *src, size_t count);
    }
)


UTEST_BEGIN("dsp", oversampling)

    void call(size_t times, const char *text, resampling_function_t func)
    {
        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 100, 999)
        {
            printf("Testing %s resampling on input buffer of %d samples...\n", text, int(count));


        }
    }

    UTEST_MAIN
    {
        // Do tests
        call(2, "2x2 native", native::lanczos_resample_2x2);
        IF_ARCH_X86(call(2, "2x2 sse", sse::lanczos_resample_2x2));

        call(2, "2x3 native", native::lanczos_resample_2x3);
        IF_ARCH_X86(call(2, "2x3 sse", sse::lanczos_resample_2x3));

        call(3, "3x2 native", native::lanczos_resample_3x2);
        IF_ARCH_X86(call(3, "3x2 sse", sse::lanczos_resample_3x2));

        call(3, "3x3 native", native::lanczos_resample_3x3);
        IF_ARCH_X86(call(3, "3x3 sse", sse::lanczos_resample_3x3));

        call(4, "4x2 native", native::lanczos_resample_4x2);
        IF_ARCH_X86(call(4, "4x2 sse", sse::lanczos_resample_4x2));

        call(4, "4x3 native", native::lanczos_resample_4x3);
        IF_ARCH_X86(call(4, "4x3 sse", sse::lanczos_resample_4x3));

        call(6, "6x2 native", native::lanczos_resample_6x2);
        IF_ARCH_X86(call(6, "6x2 sse", sse::lanczos_resample_6x2));

        call(6, "6x3 native", native::lanczos_resample_6x3);
        IF_ARCH_X86(call(6, "6x3 sse", sse::lanczos_resample_6x3));

        call(8, "8x2 native", native::lanczos_resample_8x2);
        IF_ARCH_X86(call(8, "8x2 sse", sse::lanczos_resample_8x2));

        call(8, "8x3 native", native::lanczos_resample_8x3);
        IF_ARCH_X86(call(8, "8x3 sse", sse::lanczos_resample_8x3));
    }
UTEST_END;
