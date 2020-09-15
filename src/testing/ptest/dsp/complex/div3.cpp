/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 5 сент. 2018 г.
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
#include <test/ptest.h>
#include <core/sugar.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace native
{
    void complex_div3(float *dst_re, float *dst_im, const float *t_re, const float *t_im, const float *b_re, const float *b_im, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void complex_div3(float *dst_re, float *dst_im, const float *t_re, const float *t_im, const float *b_re, const float *b_im, size_t count);
    }

    namespace avx
    {
        void complex_div3(float *dst_re, float *dst_im, const float *t_re, const float *t_im, const float *b_re, const float *b_im, size_t count);
        void complex_div3_fma3(float *dst_re, float *dst_im, const float *t_re, const float *t_im, const float *b_re, const float *b_im, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void complex_div3(float *dst_re, float *dst_im, const float *t_re, const float *t_im, const float *b_re, const float *b_im, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void complex_div3(float *dst_re, float *dst_im, const float *t_re, const float *t_im, const float *b_re, const float *b_im, size_t count);
    }
)

typedef void (* complex_div3_t) (float *dst_re, float *dst_im, const float *t_re, const float *t_im, const float *b_re, const float *b_im, size_t count);

//-----------------------------------------------------------------------------
// Performance test for complex division
PTEST_BEGIN("dsp.complex", div3, 5, 1000)

    void call(const char *label, float *dst, const float *src1, const float *src2, size_t count, complex_div3_t mul)
    {
        if (!PTEST_SUPPORTED(mul))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            mul(dst, &dst[count], src1, &src1[count], src2, &src2[count], count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *out      = alloc_aligned<float>(data, buf_size * 12, 64);
        float *in1      = &out[buf_size*2];
        float *in2      = &in1[buf_size*2];
        float *backup   = &in2[buf_size*2];

        for (size_t i=0; i < buf_size*6; ++i)
            out[i]          = float(rand()) / RAND_MAX;
        dsp::copy(backup, out, buf_size * 6);

        #define CALL(func) \
            dsp::copy(out, backup, buf_size * 6); \
            call(#func, out, in1, in2, count, func)

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(native::complex_div3);
            IF_ARCH_X86(CALL(sse::complex_div3));
            IF_ARCH_X86(CALL(avx::complex_div3));
            IF_ARCH_X86(CALL(avx::complex_div3_fma3));
            IF_ARCH_ARM(CALL(neon_d32::complex_div3));
            IF_ARCH_AARCH64(CALL(asimd::complex_div3));

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END





