/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 12 дек. 2020 г.
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
#include <test/helpers.h>
#include <test/ptest.h>
#include <core/sugar.h>

#define MIN_RANK 8
#define MAX_RANK 16

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

typedef void (* lin_inter1_t)(float *dst, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);
typedef void (* lin_inter2_t)(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);
typedef void (* lin_inter3_t)(float *dst, const float *src1, const float *src2, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n);

PTEST_BEGIN("dsp.interpolation", linear, 5, 10000)

    void call(const char *label, float *a, float *b, float *c, size_t count, lin_inter1_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(a, -1, 0.0f, 3, 0.1f, 1, count);
        );
    }

    void call(const char *label, float *a, float *b, float *c, size_t count, lin_inter2_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(a, b, -1, 0.0f, 3, 0.1f, 1, count);
        );
    }

    void call(const char *label, float *a, float *b, float *c, size_t count, lin_inter3_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(a, b, c, -1, 0.0f, 3, 0.1f, 1, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *a        = alloc_aligned<float>(data, buf_size * 3, 64);
        float *b        = &a[buf_size];
        float *c        = &a[buf_size];

        randomize_sign(a, buf_size * 3);

        #define CALL(func) \
            call(#func, a, b, c, count, func)

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(native::lin_inter_set);
            IF_ARCH_X86(CALL(sse::lin_inter_set));
            IF_ARCH_X86(CALL(avx::lin_inter_set));
            PTEST_SEPARATOR;

            CALL(native::lin_inter_mul2);
            IF_ARCH_X86(CALL(sse::lin_inter_mul2));
            IF_ARCH_X86(CALL(avx::lin_inter_mul2));
            PTEST_SEPARATOR;

            CALL(native::lin_inter_mul3);
            IF_ARCH_X86(CALL(sse::lin_inter_mul3));
            IF_ARCH_X86(CALL(avx::lin_inter_mul3));
            PTEST_SEPARATOR;

            CALL(native::lin_inter_fmadd2);
            IF_ARCH_X86(CALL(sse::lin_inter_fmadd2));
            IF_ARCH_X86(CALL(avx::lin_inter_fmadd2));
            PTEST_SEPARATOR;

            CALL(native::lin_inter_frmadd2);
            IF_ARCH_X86(CALL(sse::lin_inter_frmadd2));
            IF_ARCH_X86(CALL(avx::lin_inter_frmadd2));
            PTEST_SEPARATOR;

            CALL(native::lin_inter_fmadd3);
            IF_ARCH_X86(CALL(sse::lin_inter_fmadd3));
            IF_ARCH_X86(CALL(avx::lin_inter_fmadd3));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }

PTEST_END


