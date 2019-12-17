/*
 * exp.cpp
 *
 *  Created on: 7 дек. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/FloatBuffer.h>

namespace native
{
    void exp1(float *dst, size_t count);
    void exp2(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse2
    {
        void exp1(float *dst, size_t count);
        void exp2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_X86_64(
    namespace avx2
    {
        void x64_exp1(float *dst, size_t count);
        void x64_exp2(float *dst, const float *src, size_t count);

        void x64_exp1_fma3(float *dst, size_t count);
        void x64_exp2_fma3(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void exp1(float *dst, size_t count);
        void exp2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void exp1(float *dst, size_t count);
        void exp2(float *dst, const float *src, size_t count);
    }
)

typedef void (* exp1_t)(float *dst, size_t count);
typedef void (* exp2_t)(float *dst, const float *src, size_t count);

static inline void std_exp1(float *dst, size_t count)
{
    for (size_t i=0; i<count; ++i)
    {
        float x = dst[i];
        dst[i]  = (x < 0.0f) ? 1.0f / expf(-x) : expf(x);
    }
}

static inline void std_exp2(float *dst, const float *src, size_t count)
{
    for (size_t i=0; i<count; ++i)
    {
        float x = src[i];
        dst[i]  = (x < 0.0f) ? 1.0f / expf(-x) : expf(x);
    }
}

//-----------------------------------------------------------------------------
// Unit test
UTEST_BEGIN("dsp.pmath", exp)

    void call(const char *label, size_t align, exp1_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer src(count, align, mask & 0x01);
                src.randomize(-20.0f, 20.0f);

                FloatBuffer dst1(src);
                FloatBuffer dst2(src);

                // Call functions
                std_exp1(dst1, count);
                func(dst2, count);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_adaptive(dst2, 1e-4))
                {
                    src.dump("src ");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    void call(const char *label, size_t align, exp2_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer src(count, align, mask & 0x01);
                src.randomize(-20.0f, 20.0f);

                FloatBuffer dst1(count, align, mask & 0x02);
                FloatBuffer dst2(dst1);

                // Call functions
                std_exp2(dst1, src, count);
                func(dst2, src, count);

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_adaptive(dst2, 1e-4))
                {
                    src.dump("src ");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(func, align) \
            call(#func, align, func)

        CALL(native::exp1, 16);
        CALL(native::exp2, 16);

        IF_ARCH_X86(CALL(sse2::exp1, 16));
        IF_ARCH_X86(CALL(sse2::exp2, 16));

        IF_ARCH_X86_64(CALL(avx2::x64_exp1, 32));
        IF_ARCH_X86_64(CALL(avx2::x64_exp2, 32));

        IF_ARCH_X86_64(CALL(avx2::x64_exp1_fma3, 32));
        IF_ARCH_X86_64(CALL(avx2::x64_exp2_fma3, 32));

        IF_ARCH_ARM(CALL(neon_d32::exp1, 16));
        IF_ARCH_ARM(CALL(neon_d32::exp2, 16));

        IF_ARCH_AARCH64(CALL(asimd::exp1, 16));
        IF_ARCH_AARCH64(CALL(asimd::exp2, 16));
    }
UTEST_END





