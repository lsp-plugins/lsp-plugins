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

IF_ARCH_X86(
    namespace sse
    {
        void lin_inter_set(float *dst, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);
        void lin_inter_mul2(float *dst, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);
        void lin_inter_mul3(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);
        void lin_inter_fmadd2(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);
        void lin_inter_frmadd2(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);
        void lin_inter_fmadd3(float *dst, const float *src1, const float *src2, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);
    }

    namespace avx
    {
        void lin_inter_set(float *dst, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);
        void lin_inter_mul2(float *dst, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);
        void lin_inter_mul3(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);
        void lin_inter_fmadd2(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);
        void lin_inter_frmadd2(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);
        void lin_inter_fmadd3(float *dst, const float *src1, const float *src2, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void lin_inter_set(float *dst, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);
        void lin_inter_mul2(float *dst, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);
        void lin_inter_mul3(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);
        void lin_inter_fmadd2(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);
        void lin_inter_frmadd2(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);
        void lin_inter_fmadd3(float *dst, const float *src1, const float *src2, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);
    }
)

typedef void (* lin_inter1_t)(float *dst, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);
typedef void (* lin_inter2_t)(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);
typedef void (* lin_inter3_t)(float *dst, const float *src1, const float *src2, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);

UTEST_BEGIN("dsp.interpolation", linear)

    void call(const char *label, size_t align, lin_inter1_t func1, lin_inter1_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
                32, 64, 65, 100, 127, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer dst1(count, align, mask & 0x01);
                FloatBuffer dst2(dst1);

                // Call functions
                func1(dst1, -1, 0.0f, 3, 0.1f, 1, count);
                func2(dst2, -1, 0.0f, 3, 0.1f, 1, count);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_relative(dst2, 1e-4))
                {
                    dst1.dump("dst1 ");
                    dst2.dump("dst2 ");

                    if (dst1.last_diff() >= 0)
                        eprintf("dst1 vs dst2 index=%d, %.6f vs %.6f\n", dst1.last_diff(), dst1.get_diff(), dst2.get_diff());
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    void call(const char *label, size_t align, lin_inter2_t func1, lin_inter2_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
                32, 64, 65, 100, 127, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer src(count, align, mask & 0x01);
                FloatBuffer dst1(count, align, mask & 0x02);
                FloatBuffer dst2(dst1);

                // Call functions
                func1(dst1, src, -1, 0.0f, 3, 0.1f, 1, count);
                func2(dst2, src, -1, 0.0f, 3, 0.1f, 1, count);

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_relative(dst2, 1e-4))
                {
                    src.dump ("src  ");
                    dst1.dump("dst1 ");
                    dst2.dump("dst2 ");

                    if (dst1.last_diff() >= 0)
                        eprintf("dst1 vs dst2 index=%d, %.6f vs %.6f\n", dst1.last_diff(), dst1.get_diff(), dst2.get_diff());
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    void call(const char *label, size_t align, lin_inter3_t func1, lin_inter3_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
                32, 64, 65, 100, 127, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x07; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer src1(count, align, mask & 0x01);
                FloatBuffer src2(count, align, mask & 0x02);
                FloatBuffer dst1(count, align, mask & 0x04);
                FloatBuffer dst2(count, align, mask & 0x04);

                // Call functions
                func1(dst1, src1, src2, -1, 0.0f, 3, 0.1f, 1, count);
                func2(dst2, src1, src2, -1, 0.0f, 3, 0.1f, 1, count);

                UTEST_ASSERT_MSG(src1.valid(), "Source buffer 1 corrupted");
                UTEST_ASSERT_MSG(src2.valid(), "Source buffer 2 corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_relative(dst2, 1e-4))
                {
                    src1.dump("src1 ");
                    src2.dump("src2 ");
                    dst1.dump("dst1 ");
                    dst2.dump("dst2 ");

                    if (dst1.last_diff() >= 0)
                        eprintf("dst1 vs dst2 index=%d, %.6f vs %.6f\n", dst1.last_diff(), dst1.get_diff(), dst2.get_diff());
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

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
        // Validate the function
//        validate();

        #define CALL(func, native, align) \
            call(#func, align, native, func)

        IF_ARCH_X86(CALL(sse::lin_inter_set, native::lin_inter_set, 16));
        IF_ARCH_X86(CALL(sse::lin_inter_mul2, native::lin_inter_mul2, 16));
        IF_ARCH_X86(CALL(sse::lin_inter_mul3, native::lin_inter_mul3, 16));
        IF_ARCH_X86(CALL(sse::lin_inter_fmadd2, native::lin_inter_fmadd2, 16));
        IF_ARCH_X86(CALL(sse::lin_inter_frmadd2, native::lin_inter_frmadd2, 16));
        IF_ARCH_X86(CALL(sse::lin_inter_fmadd3, native::lin_inter_fmadd3, 16));

        IF_ARCH_X86(CALL(avx::lin_inter_set, native::lin_inter_set, 32));
        IF_ARCH_X86(CALL(avx::lin_inter_mul2, native::lin_inter_mul2, 32));
        IF_ARCH_X86(CALL(avx::lin_inter_mul3, native::lin_inter_mul3, 32));
        IF_ARCH_X86(CALL(avx::lin_inter_fmadd2, native::lin_inter_fmadd2, 32));
        IF_ARCH_X86(CALL(avx::lin_inter_frmadd2, native::lin_inter_frmadd2, 32));
        IF_ARCH_X86(CALL(avx::lin_inter_fmadd3, native::lin_inter_fmadd3, 32));

        IF_ARCH_ARM(CALL(neon_d32::lin_inter_set, native::lin_inter_set, 16));
        IF_ARCH_ARM(CALL(neon_d32::lin_inter_mul2, native::lin_inter_mul2, 16));
        IF_ARCH_ARM(CALL(neon_d32::lin_inter_mul3, native::lin_inter_mul3, 16));
        IF_ARCH_ARM(CALL(neon_d32::lin_inter_fmadd2, native::lin_inter_fmadd2, 16));
        IF_ARCH_ARM(CALL(neon_d32::lin_inter_frmadd2, native::lin_inter_frmadd2, 16));
        IF_ARCH_ARM(CALL(neon_d32::lin_inter_fmadd3, native::lin_inter_fmadd3, 16));
    }
UTEST_END



