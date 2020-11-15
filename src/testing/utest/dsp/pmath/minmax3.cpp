/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 5 июл. 2020 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/FloatBuffer.h>

namespace native
{
    void pmin3(float *dst, const float *a, const float *b, size_t count);
    void pmax3(float *dst, const float *a, const float *b, size_t count);
    void psmin3(float *dst, const float *a, const float *b, size_t count);
    void psmax3(float *dst, const float *a, const float *b, size_t count);
    void pamin3(float *dst, const float *a, const float *b, size_t count);
    void pamax3(float *dst, const float *a, const float *b, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void pmin3(float *dst, const float *a, const float *b, size_t count);
        void pmax3(float *dst, const float *a, const float *b, size_t count);
        void psmin3(float *dst, const float *a, const float *b, size_t count);
        void psmax3(float *dst, const float *a, const float *b, size_t count);
        void pamin3(float *dst, const float *a, const float *b, size_t count);
        void pamax3(float *dst, const float *a, const float *b, size_t count);
    }

    namespace avx
    {
        void pmin3(float *dst, const float *a, const float *b, size_t count);
        void pmax3(float *dst, const float *a, const float *b, size_t count);
        void psmin3(float *dst, const float *a, const float *b, size_t count);
        void psmax3(float *dst, const float *a, const float *b, size_t count);
        void pamin3(float *dst, const float *a, const float *b, size_t count);
        void pamax3(float *dst, const float *a, const float *b, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void pmin3(float *dst, const float *a, const float *b, size_t count);
        void pmax3(float *dst, const float *a, const float *b, size_t count);
        void psmin3(float *dst, const float *a, const float *b, size_t count);
        void psmax3(float *dst, const float *a, const float *b, size_t count);
        void pamin3(float *dst, const float *a, const float *b, size_t count);
        void pamax3(float *dst, const float *a, const float *b, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void pmin3(float *dst, const float *a, const float *b, size_t count);
        void pmax3(float *dst, const float *a, const float *b, size_t count);
        void psmin3(float *dst, const float *a, const float *b, size_t count);
        void psmax3(float *dst, const float *a, const float *b, size_t count);
        void pamin3(float *dst, const float *a, const float *b, size_t count);
        void pamax3(float *dst, const float *a, const float *b, size_t count);
    }
)


typedef void (* min3_t)(float *dst, const float *a, const float *b, size_t count);

//-----------------------------------------------------------------------------
// Unit test
UTEST_BEGIN("dsp.pmath", minmax3)

    void call(const char *label, size_t align, min3_t func1, min3_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x07; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer a(count, align, mask & 0x01);
                FloatBuffer b(count, align, mask & 0x02);
                a.randomize_sign();
                b.randomize_sign();

                FloatBuffer dst1(count, align, mask & 0x04);
                FloatBuffer dst2(dst1);

                // Call functions
                func1(dst1, a, b, count);
                func2(dst2, a, b, count);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_adaptive(dst2, 1e-4))
                {
                    a.dump("srca");
                    b.dump("srcb");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(native, func, align) \
            call(#func, align, native, func)

        IF_ARCH_X86(CALL(native::pmin3, sse::pmin3, 16));
        IF_ARCH_X86(CALL(native::pmax3, sse::pmax3, 16));
        IF_ARCH_X86(CALL(native::psmin3, sse::psmin3, 16));
        IF_ARCH_X86(CALL(native::psmax3, sse::psmax3, 16));
        IF_ARCH_X86(CALL(native::pamin3, sse::pamin3, 16));
        IF_ARCH_X86(CALL(native::pamax3, sse::pamax3, 16));

        IF_ARCH_X86(CALL(native::pmin3, avx::pmin3, 32));
        IF_ARCH_X86(CALL(native::pmax3, avx::pmax3, 32));
        IF_ARCH_X86(CALL(native::psmin3, avx::psmin3, 32));
        IF_ARCH_X86(CALL(native::psmax3, avx::psmax3, 32));
        IF_ARCH_X86(CALL(native::pamin3, avx::pamin3, 32));
        IF_ARCH_X86(CALL(native::pamax3, avx::pamax3, 32));

        IF_ARCH_ARM(CALL(native::pmin3, neon_d32::pmin3, 16));
        IF_ARCH_ARM(CALL(native::pmax3, neon_d32::pmax3, 16));
        IF_ARCH_ARM(CALL(native::psmin3, neon_d32::psmin3, 16));
        IF_ARCH_ARM(CALL(native::psmax3, neon_d32::psmax3, 16));
        IF_ARCH_ARM(CALL(native::pamin3, neon_d32::pamin3, 16));
        IF_ARCH_ARM(CALL(native::pamax3, neon_d32::pamax3, 16));

        IF_ARCH_AARCH64(CALL(native::pmin3, asimd::pmin3, 16));
        IF_ARCH_AARCH64(CALL(native::pmax3, asimd::pmax3, 16));
        IF_ARCH_AARCH64(CALL(native::psmin3, asimd::psmin3, 16));
        IF_ARCH_AARCH64(CALL(native::psmax3, asimd::psmax3, 16));
        IF_ARCH_AARCH64(CALL(native::pamin3, asimd::pamin3, 16));
        IF_ARCH_AARCH64(CALL(native::pamax3, asimd::pamax3, 16));
    }
UTEST_END
