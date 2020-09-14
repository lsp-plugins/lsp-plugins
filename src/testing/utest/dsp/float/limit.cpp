/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 17 дек. 2018 г.
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
    void limit1(float *dst, float min, float max, size_t count);
    void limit2(float *dst, const float *src, float min, float max, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void limit1(float *dst, float min, float max, size_t count);
        void limit2(float *dst, const float *src, float min, float max, size_t count);
    }

    namespace avx
    {
        void limit1(float *dst, float min, float max, size_t count);
        void limit2(float *dst, const float *src, float min, float max, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void limit1(float *dst, float min, float max, size_t count);
        void limit2(float *dst, const float *src, float min, float max, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void limit1(float *dst, float min, float max, size_t count);
        void limit2(float *dst, const float *src, float min, float max, size_t count);
    }
)

typedef void (* limit1_t)(float *dst, float min, float max, size_t count);
typedef void (* limit2_t)(float *dst, const float *src, float min, float max, size_t count);

UTEST_BEGIN("dsp.float", limit)

    void init_buf(FloatBuffer &buf)
    {
        for (size_t i=0; i<buf.size(); ++i)
        {
            switch (i%6)
            {
                case 0:
                    buf[i]          = +INFINITY;
                    break;
                case 1:
                    buf[i]          = (rand() * 2.0f) / RAND_MAX;
                    break;
                case 2:
                    buf[i]          = -INFINITY;
                    break;
                case 3:
                    buf[i]          = - (rand() * 2.0f) / RAND_MAX;
                    break;
                case 4:
                    buf[i]          = -NAN;
                    break;
                default:
                    buf[i]          = NAN;
                    break;
            }
        }
    }

    void check_buffer(const char *label, FloatBuffer & buf)
    {
        UTEST_ASSERT_MSG(buf.valid(), "Destination buffer '%s' corrupted", label);
        for (size_t i=0, n=buf.size(); i<n; ++i)
        {
            float s = buf.get(i);
            UTEST_ASSERT_MSG((s >= -1.0f) && (s <= 1.0f), "Invalid buffer contents: %.5f", s);
        }
    }

    void call(const char *label, size_t align, limit2_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                        32, 64, 65, 100, 768, 999, 1024, 0x1fff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer src(count, align, mask & 0x01);
                init_buf(src);
                FloatBuffer dst1(count, align, mask & 0x02);
                FloatBuffer dst2(dst1);

                // Call functions
                native::limit2(dst1, src, -1.0f, 1.0f, count);
                func(dst2, src, -1.0f, 1.0f, count);

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");

                check_buffer("dst1", dst1);
                check_buffer("dst2", dst2);

                // Compare buffers
                if (!dst1.equals_relative(dst2, 1e-5))
                {
                    src.dump("src ");
                    src.dump_hex("srch");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    void call(const char *label, size_t align, limit1_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 768, 999, 1024, 0x1fff)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer dst1(count, align, mask & 0x02);
                init_buf(dst1);
                FloatBuffer dst2(dst1);
                FloatBuffer src(dst1);

                // Call functions
                native::limit1(dst1, -1.0f, 1.0f, count);
                func(dst2, -1.0f, 1.0f, count);

                check_buffer("dst1", dst1);
                check_buffer("dst2", dst2);

                // Compare buffers
                if (!dst1.equals_relative(dst2, 1e-5))
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

        IF_ARCH_X86(CALL(sse::limit1, 16));
        IF_ARCH_X86(CALL(sse::limit2, 16));

        IF_ARCH_X86(CALL(avx::limit1, 32));
        IF_ARCH_X86(CALL(avx::limit2, 32));

        IF_ARCH_ARM(CALL(neon_d32::limit1, 16));
        IF_ARCH_ARM(CALL(neon_d32::limit2, 16));

        IF_ARCH_AARCH64(CALL(asimd::limit1, 16));
        IF_ARCH_AARCH64(CALL(asimd::limit2, 16));
    }

UTEST_END;


