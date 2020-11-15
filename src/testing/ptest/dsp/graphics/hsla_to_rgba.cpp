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
#include <test/ptest.h>
#include <core/sugar.h>

#define MIN_RANK 6
#define SCALE_RANK 12
#define MAX_RANK 16

namespace native
{
    void hsla_to_rgba(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse2
    {
        void hsla_to_rgba(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void hsla_to_rgba(float *dst, const float *src, size_t count);
    }
)

typedef void (* hsla_to_rgba_t)(float *dst, const float *src, size_t count);

//-----------------------------------------------------------------------------
// Performance test for logarithmic axis calculation
PTEST_BEGIN("dsp.graphics", hsla_to_rgba, 5, 5000)

    void call(const char *label, float *dst, float *src, size_t count, size_t scale, hsla_to_rgba_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s pixels...\n", buf);

        PTEST_KLOOP(buf, scale,
            func(dst, src, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size     = 1 << MAX_RANK;
        uint8_t *data       = NULL;
        float *src          = alloc_aligned<float>(data, (buf_size * 2) * 4, 64);
        float *dst          = &src[buf_size * 4];

        for (size_t i=0; i<buf_size*4; ++i)
             src[i]             = float(rand()) / (RAND_MAX - 1);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;
            size_t scale = (i < SCALE_RANK) ? (1 << (SCALE_RANK - i)) : 1;

            call("native::hsla_to_rgba", dst, src, count, scale, native::hsla_to_rgba);
            IF_ARCH_X86(call("sse2::hsla_to_rgba", dst, src, count, scale, sse2::hsla_to_rgba));
            IF_ARCH_ARM(call("neon_d32::hsla_to_rgba", dst, src, count, scale, neon_d32::hsla_to_rgba));

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END



