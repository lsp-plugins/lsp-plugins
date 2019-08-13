/*
 * pfft.cpp
 *
 *  Created on: 31 окт. 2018 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <test/FloatBuffer.h>
#include <dsp/dsp.h>

#define TOLERANCE       5e-2

namespace native
{
    void packed_direct_fft(float *dst, const float *src, size_t rank);
    void packed_reverse_fft(float *dst, const float *src, size_t rank);
}

IF_ARCH_X86(
    namespace sse
    {
        void packed_direct_fft(float *dst, const float *src, size_t rank);
        void packed_reverse_fft(float *dst, const float *src, size_t rank);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void packed_direct_fft(float *dst, const float *src, size_t rank);
        void packed_reverse_fft(float *dst, const float *src, size_t rank);
    }
)

typedef void (* packed_fft_t)(float *dst, const float *src, size_t rank);

UTEST_BEGIN("dsp.fft", pfft)

    void call(const char *label, size_t align, packed_fft_t func1, packed_fft_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        for (int same=0; same < 2; ++same)
        {
            for (size_t rank=6; rank<=16; ++rank)
            {
                size_t count = 1 << (rank + 1);
                for (size_t mask=0; mask <= 0x03; ++mask)
                {
                    FloatBuffer src(count, align, mask & 0x01);
                    FloatBuffer dst1(count, align, mask & 0x02);
                    FloatBuffer dst2(dst1);

                    printf("Testing '%s' for rank=%d, mask=0x%x, same=%s...\n", label, int(rank), int(mask), (same) ? "true" : "false");

                    if (same)
                    {
                        dsp::copy(dst1, src, count);
                        dsp::copy(dst2, src, count);
                        func1(dst1, dst1, rank);
                        func2(dst2, dst2, rank);
                    }
                    else
                    {
                        func1(dst1, src, rank);
                        func2(dst2, src, rank);
                    }

                    UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                    UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                    UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                    // Compare buffers
                    if ((!dst1.equals_adaptive(dst2, TOLERANCE)))
                    {
                        ssize_t diff = dst1.last_diff();
                        src.dump("src ");
                        dst1.dump("dst1");
                        dst2.dump("dst2");
                        UTEST_FAIL_MSG("Output of functions for test '%s' differs at sample %d (%.5f vs %.5f)",
                                label, int(diff), dst1.get(diff), dst2.get(diff));
                    }
                }
            }
        }
    }

    UTEST_MAIN
    {
        // Do tests
        IF_ARCH_X86(call("sse::packed_direct_fft", 16, native::packed_direct_fft, sse::packed_direct_fft));
        IF_ARCH_X86(call("sse::packed_reverse_fft", 16, native::packed_reverse_fft, sse::packed_reverse_fft));

        IF_ARCH_ARM(call("neon_d32::packed_direct_fft", 16, native::packed_direct_fft, neon_d32::packed_direct_fft));
        IF_ARCH_ARM(call("neon_d32::packed_reverse_fft", 16, native::packed_reverse_fft, neon_d32::packed_reverse_fft));
    }
UTEST_END;
