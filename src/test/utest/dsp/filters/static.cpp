/*
 * bq_static.cpp
 *
 *  Created on: 12 окт. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/FloatBuffer.h>

#define BUF_SIZE        1024
#define TOLERANCE       1e-4f

namespace native
{
    void biquad_process_x1(float *dst, const float *src, size_t count, biquad_t *f);
    void biquad_process_x2(float *dst, const float *src, size_t count, biquad_t *f);
    void biquad_process_x4(float *dst, const float *src, size_t count, biquad_t *f);
    void biquad_process_x8(float *dst, const float *src, size_t count, biquad_t *f);
}

IF_ARCH_X86(
    namespace sse
    {
        void biquad_process_x1(float *dst, const float *src, size_t count, biquad_t *f);
        void biquad_process_x2(float *dst, const float *src, size_t count, biquad_t *f);
        void biquad_process_x4(float *dst, const float *src, size_t count, biquad_t *f);
        void biquad_process_x8(float *dst, const float *src, size_t count, biquad_t *f);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void biquad_process_x1(float *dst, const float *src, size_t count, biquad_t *f);
//        void biquad_process_x2(float *dst, const float *src, size_t count, biquad_t *f);
//        void biquad_process_x4(float *dst, const float *src, size_t count, biquad_t *f);
//        void biquad_process_x8(float *dst, const float *src, size_t count, biquad_t *f);
    }
)

typedef void (* biquad_process_t)(float *dst, const float *src, size_t count, biquad_t *f);

UTEST_BEGIN("dsp.filters", static)

    void call(const char *label, const biquad_t *bq, biquad_process_t func1, biquad_process_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        printf("Testing %s on buffer size %d...\n", label, BUF_SIZE);

        biquad_t f1 = *bq, f2 = *bq;

        FloatBuffer src(BUF_SIZE);
        FloatBuffer dst1(BUF_SIZE);
        FloatBuffer dst2(BUF_SIZE);

        func1(dst1, src, BUF_SIZE, &f1);
        func2(dst2, src, BUF_SIZE, &f2);

        UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
        UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
        UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");
        if (!dst1.equals_absolute(dst2, TOLERANCE))
        {
            src.dump("src");
            dst1.dump("dst1");
            dst2.dump("dst2");
            UTEST_FAIL_MSG("Output of functions for test '%s' differs at sample %d: %.6f vs %.6f",
                    label, int(dst1.last_diff()), dst1.get_diff(), dst2.get_diff());
        }
    }


    UTEST_MAIN
    {
        biquad_t bq __lsp_aligned64;
        dsp::fill_zero(bq.d, BIQUAD_D_ITEMS);

        // Prepare simple 2 zero, 2 pole hi-pass filter
        biquad_x1_t *x1 = &bq.x1;
        x1->a[0] = 0.992303491f;
        x1->a[1] = 0.992303491f;
        x1->a[2] = -1.98460698f;
        x1->a[3] = 0.992303491f;

        x1->b[0] = 1.98398674f;
        x1->b[1] = -0.985227287f;
        x1->b[2] = 0.0f;
        x1->b[3] = 0.0f;

        IF_ARCH_X86(call("sse::biquad_process_x1", &bq, native::biquad_process_x1, sse::biquad_process_x1));
        IF_ARCH_ARM(call("neon_d32::biquad_process_x1", &bq, native::biquad_process_x1, neon_d32::biquad_process_x1));
    }

UTEST_END
