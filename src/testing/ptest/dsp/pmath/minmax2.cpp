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
#include <test/ptest.h>
#include <test/helpers.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace native
{
    void pmin2(float *dst, const float *src, size_t count);
    void pmax2(float *dst, const float *src, size_t count);
    void psmin2(float *dst, const float *src, size_t count);
    void psmax2(float *dst, const float *src, size_t count);
    void pamin2(float *dst, const float *src, size_t count);
    void pamax2(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void pmin2(float *dst, const float *src, size_t count);
        void pmax2(float *dst, const float *src, size_t count);
        void psmin2(float *dst, const float *src, size_t count);
        void psmax2(float *dst, const float *src, size_t count);
        void pamin2(float *dst, const float *src, size_t count);
        void pamax2(float *dst, const float *src, size_t count);
    }

    namespace avx
    {
        void pmin2(float *dst, const float *src, size_t count);
        void pmax2(float *dst, const float *src, size_t count);
        void psmin2(float *dst, const float *src, size_t count);
        void psmax2(float *dst, const float *src, size_t count);
        void pamin2(float *dst, const float *src, size_t count);
        void pamax2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void pmin2(float *dst, const float *src, size_t count);
        void pmax2(float *dst, const float *src, size_t count);
        void psmin2(float *dst, const float *src, size_t count);
        void psmax2(float *dst, const float *src, size_t count);
        void pamin2(float *dst, const float *src, size_t count);
        void pamax2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void pmin2(float *dst, const float *src, size_t count);
        void pmax2(float *dst, const float *src, size_t count);
        void psmin2(float *dst, const float *src, size_t count);
        void psmax2(float *dst, const float *src, size_t count);
        void pamin2(float *dst, const float *src, size_t count);
        void pamax2(float *dst, const float *src, size_t count);
    }
)

typedef void (* min2_t)(float *dst, const float *src, size_t count);

//-----------------------------------------------------------------------------
PTEST_BEGIN("dsp.pmath", minmax2, 5, 10000)

    void call(const char *label, float *dst, const float *src, size_t count, min2_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(dst, src, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size * 4, 64);
        float *src      = &dst[buf_size];
        float *backup   = &src[buf_size];

        randomize_sign(dst, buf_size*2);
        dsp::copy(backup, dst, buf_size*2);

        #define CALL(method) \
            dsp::copy(dst, backup, buf_size*2); \
            call(#method, dst, src, count, method);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(native::pmin2);
            IF_ARCH_X86(CALL(sse::pmin2));
            IF_ARCH_X86(CALL(avx::pmin2));
            IF_ARCH_ARM(CALL(neon_d32::pmin2));
            IF_ARCH_AARCH64(CALL(asimd::pmin2));
            PTEST_SEPARATOR;

            CALL(native::pmax2);
            IF_ARCH_X86(CALL(sse::pmax2));
            IF_ARCH_X86(CALL(avx::pmax2));
            IF_ARCH_ARM(CALL(neon_d32::pmax2));
            IF_ARCH_AARCH64(CALL(asimd::pmax2));
            PTEST_SEPARATOR;

            CALL(native::psmin2);
            IF_ARCH_X86(CALL(sse::psmin2));
            IF_ARCH_X86(CALL(avx::psmin2));
            IF_ARCH_ARM(CALL(neon_d32::psmin2));
            IF_ARCH_AARCH64(CALL(asimd::psmin2));
            PTEST_SEPARATOR;

            CALL(native::psmax2);
            IF_ARCH_X86(CALL(sse::psmax2));
            IF_ARCH_X86(CALL(avx::psmax2));
            IF_ARCH_ARM(CALL(neon_d32::psmax2));
            IF_ARCH_AARCH64(CALL(asimd::psmax2));
            PTEST_SEPARATOR;

            CALL(native::pamin2);
            IF_ARCH_X86(CALL(sse::pamin2));
            IF_ARCH_X86(CALL(avx::pamin2));
            IF_ARCH_ARM(CALL(neon_d32::pamin2));
            IF_ARCH_AARCH64(CALL(asimd::pamin2));
            PTEST_SEPARATOR;

            CALL(native::pamax2);
            IF_ARCH_X86(CALL(sse::pamax2));
            IF_ARCH_X86(CALL(avx::pamax2));
            IF_ARCH_ARM(CALL(neon_d32::pamax2));
            IF_ARCH_AARCH64(CALL(asimd::pamax2));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END
