/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 17 сент. 2018 г.
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

#define TOLERANCE 1e-2

namespace native
{
    void axis_apply_log1(float *x, const float *v, float zero, float norm_x, size_t count);
    void axis_apply_log2(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count);
}

IF_ARCH_X86(
    namespace sse2
    {
        void axis_apply_log1(float *x, const float *v, float zero, float norm_x, size_t count);
        void axis_apply_log2(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count);
    }

    IF_ARCH_X86_64(
        namespace sse3
        {
            void x64_axis_apply_log1(float *x, const float *v, float zero, float norm_x, size_t count);
            void x64_axis_apply_log2(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count);
        }
    )
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void axis_apply_log1(float *x, const float *v, float zero, float norm_x, size_t count);
        void axis_apply_log2(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count);
    }
)

typedef void (* axis_apply_log1_t)(float *x, const float *v, float zero, float norm_x, size_t count);
typedef void (* axis_apply_log2_t)(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count);

UTEST_BEGIN("dsp.graphics", axis)
    void call(const char *label, size_t align, axis_apply_log1_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        for (size_t mask=0; mask <= 0x03; ++mask)
        {
            UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0x1ff)
            {
                FloatBuffer v(count, align, mask & 0x01);
                FloatBuffer x1(count, align, mask & 0x02);
                FloatBuffer x2(x1);

                for (size_t i=0; i<count; ++i)
                    v[i] += 0.001f;

                printf("Tesing %s count=%d mask=0x%x\n", label, int(count), int(mask));

                native::axis_apply_log1(x1, v, 0.1f, 0.5f, count);
                func(x2, v, 0.1f, 0.5f, count);

                UTEST_ASSERT_MSG(v.valid(), "v corrupted");
                UTEST_ASSERT_MSG(x1.valid(), "x1 corrupted");
                UTEST_ASSERT_MSG(x2.valid(), "x2 corrupted");

                // Compare buffers
                if (!x1.equals_relative(x2, TOLERANCE))
                {
                    v.dump("v ");
                    x1.dump("x1");
                    x2.dump("x2");
                    printf("x1=%.6f vs x2=%.6f\n", x1.get_diff(), x2.get_diff());
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    void call(const char *label, size_t align, axis_apply_log2_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        for (size_t mask=0; mask <= 0x07; ++mask)
        {
            UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0x1ff)
            {
                FloatBuffer v(count, align, mask & 0x01);
                FloatBuffer x1(count, align, mask & 0x02);
                FloatBuffer y1(count, align, mask & 0x04);
                FloatBuffer x2(x1);
                FloatBuffer y2(y1);

                for (size_t i=0; i<count; ++i)
                    v[i] += 0.001f;

                printf("Tesing %s count=%d mask=0x%x\n", label, int(count), int(mask));

                native::axis_apply_log2(x1, y1, v, 0.1f, 0.5f, 1.0f, count);
                func(x2, y2, v, 0.1f, 0.5f, 1.0f, count);

                UTEST_ASSERT_MSG(v.valid(), "v corrupted");
                UTEST_ASSERT_MSG(x1.valid(), "x1 corrupted");
                UTEST_ASSERT_MSG(y1.valid(), "y1 corrupted");
                UTEST_ASSERT_MSG(x2.valid(), "x2 corrupted");
                UTEST_ASSERT_MSG(y2.valid(), "y2 corrupted");

                // Compare buffers
                if ((!x1.equals_relative(x2, TOLERANCE)) || ((!y1.equals_relative(y2, TOLERANCE))))
                {
                    v.dump("v ");
                    x1.dump("x1");
                    y1.dump("y1");
                    x2.dump("x2");
                    y2.dump("y2");
                    if (x1.last_diff() >= 0)
                        printf("x1=%.6f vs x2=%.6f\n", x1.get_diff(), x2.get_diff());
                    if (y1.last_diff() >= 0)
                        printf("y1=%.6f vs y2=%.6f\n", y2.get_diff(), y2.get_diff());
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(func, align) \
            call(#func, align, func)

        IF_ARCH_X86(CALL(sse2::axis_apply_log1, 16));
        IF_ARCH_X86(CALL(sse2::axis_apply_log2, 16));
        IF_ARCH_X86_64(CALL(sse3::x64_axis_apply_log1, 16));
        IF_ARCH_X86_64(CALL(sse3::x64_axis_apply_log2, 16));

        IF_ARCH_ARM(CALL(neon_d32::axis_apply_log1, 16));
        IF_ARCH_ARM(CALL(neon_d32::axis_apply_log2, 16));
    }

UTEST_END;
