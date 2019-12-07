/*
 * mod.cpp
 *
 *  Created on: 30 авг. 2018 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <test/FloatBuffer.h>

namespace native
{
    void pcomplex_mod(float *dst_mod, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void pcomplex_mod(float *dst_mod, const float *src, size_t count);
    }

    namespace sse3
    {
        void pcomplex_mod(float *dst_mod, const float *src, size_t count);
        void x64_pcomplex_mod(float *dst_mod, const float *src, size_t count);
    }

    namespace avx
    {
        void pcomplex_mod(float *dst_mod, const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void pcomplex_mod(float *dst_mod, const float *src, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void pcomplex_mod(float *dst_mod, const float *src, size_t count);
    }
)

typedef void (* pcomplex_mod_t)(float *dst_mod, const float *src, size_t count);

UTEST_BEGIN("dsp.pcomplex", mod)

    void call(const char *text,  size_t align, pcomplex_mod_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
                32, 33, 37, 48, 49, 64, 65, 0x3f, 100, 999, 0x1fff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", text, int(count), int(mask));

                FloatBuffer src(count*2, align, mask & 0x01);
                FloatBuffer dst1(count, align, mask & 0x02);
                FloatBuffer dst2(dst1);

                // Call functions
                native::pcomplex_mod(dst1, src, count);
                func(dst2, src, count);

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_absolute(dst2, 1e-5))
                {
                    src.dump("src ");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", text);
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(func, align) \
            call(#func, align, func)

        IF_ARCH_X86(CALL(sse::pcomplex_mod, 16));
        IF_ARCH_X86(CALL(sse3::pcomplex_mod, 16));
        IF_ARCH_X86(CALL(sse3::x64_pcomplex_mod, 16));
        IF_ARCH_X86(CALL(avx::pcomplex_mod, 32));
        IF_ARCH_ARM(CALL(neon_d32::pcomplex_mod, 16));
        IF_ARCH_AARCH64(CALL(asimd::pcomplex_mod, 16));
    }

UTEST_END


