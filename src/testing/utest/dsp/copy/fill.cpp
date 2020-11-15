/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 30 авг. 2018 г.
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

#include <test/utest.h>
#include <core/stdlib/math.h>
#include <test/FloatBuffer.h>

namespace native
{
    void fill(float *dst, float value, size_t count);
    void fill_one(float *dst, size_t count);
    void fill_zero(float *dst, size_t count);
    void fill_minus_one(float *dst, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void fill(float *dst, float value, size_t count);
        void fill_one(float *dst, size_t count);
        void fill_zero(float *dst, size_t count);
        void fill_minus_one(float *dst, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void fill(float *dst, float value, size_t count);
        void fill_one(float *dst, size_t count);
        void fill_zero(float *dst, size_t count);
        void fill_minus_one(float *dst, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void fill(float *dst, float value, size_t count);
        void fill_one(float *dst, size_t count);
        void fill_zero(float *dst, size_t count);
        void fill_minus_one(float *dst, size_t count);
    }
)

typedef void (* fill_t)(float *dst, float value, size_t count);
typedef void (* fill_value_t)(float *dst, size_t count);

UTEST_BEGIN("dsp.copy", fill)
    void call(const char *label, size_t align, fill_t func1, fill_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0x100, 0x1ff, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                FloatBuffer dst1(count, align, mask & 0x01);
                FloatBuffer dst2(count, align, mask & 0x01);

                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));
                func1(dst1, M_PI, count);
                func2(dst2, M_PI, count);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                if (!dst1.equals_absolute(dst2))
                {
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    void call(const char *label, size_t align, fill_value_t func1, fill_value_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0x100, 0x1ff, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                FloatBuffer dst1(count, align, mask & 0x01);
                FloatBuffer dst2(count, align, mask & 0x01);

                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));
                func1(dst1, count);
                func2(dst2, count);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                if (!dst1.equals_absolute(dst2))
                {
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

        IF_ARCH_X86(CALL(native::fill, sse::fill, 16));
        IF_ARCH_X86(CALL(native::fill_one, sse::fill_one, 16));
        IF_ARCH_X86(CALL(native::fill_zero, sse::fill_zero, 16));
        IF_ARCH_X86(CALL(native::fill_minus_one, sse::fill_minus_one, 16));

        IF_ARCH_ARM(CALL(native::fill, neon_d32::fill, 16));
        IF_ARCH_ARM(CALL(native::fill_one, neon_d32::fill_one, 16));
        IF_ARCH_ARM(CALL(native::fill_zero, neon_d32::fill_zero, 16));
        IF_ARCH_ARM(CALL(native::fill_minus_one, neon_d32::fill_minus_one, 16));

        IF_ARCH_AARCH64(CALL(native::fill, asimd::fill, 16));
        IF_ARCH_AARCH64(CALL(native::fill_one, asimd::fill_one, 16));
        IF_ARCH_AARCH64(CALL(native::fill_zero, asimd::fill_zero, 16));
        IF_ARCH_AARCH64(CALL(native::fill_minus_one, asimd::fill_minus_one, 16));
    }

UTEST_END;


