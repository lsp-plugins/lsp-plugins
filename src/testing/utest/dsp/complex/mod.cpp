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
#include <test/FloatBuffer.h>

namespace native
{
    void complex_mod(float *dst_mod, const float *src_re, const float *src_im, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void complex_mod(float *dst_mod, const float *src_re, const float *src_im, size_t count);
    }

    namespace avx
    {
        void complex_mod(float *dst_mod, const float *src_re, const float *src_im, size_t count);
        void complex_mod_fma3(float *dst_mod, const float *src_re, const float *src_im, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void complex_mod(float *dst_mod, const float *src_re, const float *src_im, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void complex_mod(float *dst_mod, const float *src_re, const float *src_im, size_t count);
    }
)

typedef void (* pcomplex_mod_t)(float *dst_mod, const float *src, size_t count);
typedef void (* complex_mod_t)(float *dst_mod, const float *src_re, const float *src_im, size_t count);

UTEST_BEGIN("dsp.complex", mod)

    void call(const char *text,  size_t align, complex_mod_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
                32, 33, 37, 48, 49, 64, 65, 0x3f, 100, 999, 0x1fff)
        {
            for (size_t mask=0; mask <= 0x07; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", text, int(count), int(mask));

                FloatBuffer src_re(count, align, mask & 0x01);
                FloatBuffer src_im(count, align, mask & 0x02);
                FloatBuffer dst1(count, align, mask & 0x04);
                FloatBuffer dst2(dst1);

                // Call functions
                native::complex_mod(dst1, src_re, src_im, count);
                func(dst2, src_re, src_im, count);

                UTEST_ASSERT_MSG(src_re.valid(), "Source buffer RE corrupted");
                UTEST_ASSERT_MSG(src_im.valid(), "Source buffer IM corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_absolute(dst2, 1e-5))
                {
                    src_re.dump("src_re");
                    src_im.dump("src_im");
                    dst1.dump("dst1  ");
                    dst2.dump("dst2  ");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", text);
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(func, align) \
            call(#func, align, func)

        IF_ARCH_X86(CALL(sse::complex_mod, 16));
        IF_ARCH_X86(CALL(avx::complex_mod, 16));
        IF_ARCH_X86(CALL(avx::complex_mod_fma3, 16));
        IF_ARCH_ARM(CALL(neon_d32::complex_mod, 16));
        IF_ARCH_AARCH64(CALL(asimd::complex_mod, 16));
    }

UTEST_END


