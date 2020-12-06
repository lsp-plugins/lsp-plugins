/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 7 дек. 2020 г.
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
#include <test/helpers.h>

namespace native
{
    void lin_inter_set(float *dst, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);
    void lin_inter_mul2(float *dst, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);
    void lin_inter_mul3(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);
    void lin_inter_fmadd2(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);
    void lin_inter_frmadd2(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);
    void lin_inter_fmadd3(float *dst, const float *src1, const float *src2, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);
}

UTEST_BEGIN("dsp.interpolation", linear)

    void validate()
    {
        FloatBuffer dst(16);
        FloatBuffer src1(16);
        FloatBuffer src2(16);

        // Validate lin_inter_set
        dst.randomize(-1.0f, 1.0f);
        native::lin_inter_set(dst, 0, 1.0f, 32, 2.0f, 8, 16);
        UTEST_ASSERT(!dst.corrupted());
        for (size_t i=0; i<16; ++i)
        {
            float v = 1.25f + 0.03125f * i;
            if (!float_equals_absolute(dst.get(i), v))
            {
                dst.dump("dst");
                UTEST_FAIL_MSG("Failed lin_inter_set");
            }
        }

        // Validate lin_inter_mul2
        dst.fill(2.0f);
        native::lin_inter_mul2(dst, 0, 1.0f, 32, 2.0f, 8, 16);
        UTEST_ASSERT(!dst.corrupted());
        for (size_t i=0; i<16; ++i)
        {
            float v = 2.5f + 0.0625f * i;
            if (!float_equals_absolute(dst.get(i), v))
            {
                dst.dump("dst");
                UTEST_FAIL_MSG("Failed lin_inter_mul2");
            }
        }

        // Validate lin_inter_mul3
        src1.fill(2.0f);
        dst.randomize(-1.0f, 1.0f);
        native::lin_inter_mul3(dst, src1, 0, 1.0f, 32, 2.0f, 8, 16);
        UTEST_ASSERT(!dst.corrupted());
        UTEST_ASSERT(!src1.corrupted());
        for (size_t i=0; i<16; ++i)
        {
            float v = 2.5f + 0.0625f * i;
            if (!float_equals_absolute(dst.get(i), v))
            {
                src1.dump("src1");
                dst.dump("dst");
                UTEST_FAIL_MSG("Failed lin_inter_mul3");
            }
        }

        // Validate lin_inter_fmadd2
        src1.fill(2.0f);
        dst.fill(4.0f);
        native::lin_inter_fmadd2(dst, src1, 0, 1.0f, 32, 2.0f, 8, 16);
        UTEST_ASSERT(!dst.corrupted());
        UTEST_ASSERT(!src1.corrupted());
        for (size_t i=0; i<16; ++i)
        {
            float v = 6.5f + 0.0625f * i;
            if (!float_equals_absolute(dst.get(i), v))
            {
                src1.dump("src1");
                dst.dump("dst");
                UTEST_FAIL_MSG("Failed lin_inter_fmadd2");
            }
        }

        // Validate lin_inter_frmadd2
        src1.fill(2.0f);
        dst.fill(4.0f);
        native::lin_inter_frmadd2(dst, src1, 0, 1.0f, 32, 2.0f, 8, 16);
        UTEST_ASSERT(!dst.corrupted());
        UTEST_ASSERT(!src1.corrupted());
        for (size_t i=0; i<16; ++i)
        {
            float v = 7.0f + 0.125f * i;
            if (!float_equals_absolute(dst.get(i), v))
            {
                src1.dump("src1");
                dst.dump("dst");
                UTEST_FAIL_MSG("Failed lin_inter_frmadd2");
            }
        }

        // Validate lin_inter_fmadd3
        src1.fill(2.0f);
        src2.fill(4.0f);
        dst.randomize(-1.0f, 1.0f);
        native::lin_inter_fmadd3(dst, src1, src2, 0, 1.0f, 32, 2.0f, 8, 16);
        UTEST_ASSERT(!dst.corrupted());
        UTEST_ASSERT(!src1.corrupted());
        UTEST_ASSERT(!src2.corrupted());
        for (size_t i=0; i<16; ++i)
        {
            float v = 6.5f + 0.0625f * i;
            if (!float_equals_absolute(dst.get(i), v))
            {
                src1.dump("src1");
                src2.dump("src2");
                dst.dump("dst");
                UTEST_FAIL_MSG("Failed lin_inter_fmadd3");
            }
        }
    }

    UTEST_MAIN
    {
        validate();
    }
UTEST_END



