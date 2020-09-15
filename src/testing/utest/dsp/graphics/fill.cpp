/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 16 нояб. 2018 г.
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

UTEST_BEGIN("dsp.graphics", fill)
    void call(const char *label, size_t align, hsla_to_fill_t func1, hsla_to_fill_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 768, 999, 1024, 0x1fff)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer dst1(count*4, align, mask & 0x01);
                FloatBuffer dst2(count*4, align, mask & 0x01);

                float c1 = rand() / (RAND_MAX-1);
                float c2 = rand() / (RAND_MAX-1);
                float c3 = rand() / (RAND_MAX-1);
                float c4 = rand() / (RAND_MAX-1);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Call functions
                func1(dst1, c1, c2, c3, c4, count);
                func2(dst2, c1, c2, c3, c4, count);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_absolute(dst2, 1e-5f))
                {
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Result of functions differs at sample %d: %.5f vs %.5f",
                        int(dst1.last_diff()), dst1.get_diff(), dst2.get_diff());
                }
            }
        }
    }

    UTEST_MAIN
    {
        IF_ARCH_X86(call("sse::fill_rgba", 16, native::fill_rgba, sse::fill_rgba));
        IF_ARCH_X86(call("sse::fill_hsla", 16, native::fill_hsla, sse::fill_hsla));

        IF_ARCH_ARM(call("neon_d32::fill_rgba", 16, native::fill_rgba, neon_d32::fill_rgba));
        IF_ARCH_ARM(call("neon_d32::fill_hsla", 16, native::fill_hsla, neon_d32::fill_hsla));
    }

UTEST_END;


