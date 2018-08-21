/*
 * oversampling.cpp
 *
 *  Created on: 21 авг. 2018 г.
 *      Author: vsadovnikov
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/FloatBuffer.h>

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

    void call(size_t times, const char *text, size_t align,
            resampling_function_t func1,
            resampling_function_t func2
         )
    {
        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 100, 999)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                printf("Testing %s resampling on input buffer of %d samples, mask=0x%x...\n", text, int(count), int(mask));

                FloatBuffer src1(count, align, mask & 0x01);
                FloatBuffer src2(src1);
                FloatBuffer dst1(count*times + RESAMPLING_RESERVED_SAMPLES, align, mask & 0x02);
                FloatBuffer dst2(dst1);

                // Call functions
                func1(dst1, src1, count);
                func2(dst2, src2, count);

                if (src1.corrupted())
                    UTEST_FAIL_MSG("Source buffer 1 corrupted");
                if (src2.corrupted())
                    UTEST_FAIL_MSG("Source buffer 2 corrupted");
                if (dst1.corrupted())
                    UTEST_FAIL_MSG("Destination buffer 1 corrupted");
                if (dst2.corrupted())
                    UTEST_FAIL_MSG("Destination buffer 2 corrupted");

                // Compare buffers
                UTEST_ASSERT_MSG(dst1.equals(dst2), "Output of functions for test %s difffers", text);
            }
        }
    }

    UTEST_MAIN
    {
        // Do tests
        IF_ARCH_X86(call(2, "2x2 sse", 16, native::lanczos_resample_2x2, sse::lanczos_resample_2x2));
        IF_ARCH_X86(call(2, "2x3 sse", 16, native::lanczos_resample_2x3, sse::lanczos_resample_2x3));
        IF_ARCH_X86(call(3, "3x2 sse", 16, native::lanczos_resample_3x2, sse::lanczos_resample_3x2));
        IF_ARCH_X86(call(3, "3x3 sse", 16, native::lanczos_resample_3x3, sse::lanczos_resample_3x3));
        IF_ARCH_X86(call(4, "4x2 sse", 16, native::lanczos_resample_4x2, sse::lanczos_resample_4x2));
        IF_ARCH_X86(call(4, "4x3 sse", 16, native::lanczos_resample_4x3, sse::lanczos_resample_4x3));
        IF_ARCH_X86(call(6, "6x2 sse", 16, native::lanczos_resample_6x2, sse::lanczos_resample_6x2));
        IF_ARCH_X86(call(6, "6x3 sse", 16, native::lanczos_resample_6x3, sse::lanczos_resample_6x3));
        IF_ARCH_X86(call(8, "8x2 sse", 16, native::lanczos_resample_8x2, sse::lanczos_resample_8x2));
        IF_ARCH_X86(call(8, "8x3 sse", 16, native::lanczos_resample_8x3, sse::lanczos_resample_8x3));
    }
UTEST_END;
