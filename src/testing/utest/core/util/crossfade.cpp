/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 22 янв. 2020 г.
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
#include <test/helpers.h>
#include <core/util/Crossfade.h>

#define SRATE       64
#define BUF_SIZE    (SRATE * 3)
#define BUF_STEP    8

using namespace lsp;

UTEST_BEGIN("core.util", crossfade)

    void process(Crossfade &cf, float *dst, float *out, float *in)
    {
        cf.reset();
        for (size_t i=0; i<BUF_SIZE; i += BUF_STEP)
        {
            // Toggle crossfade if needed
            if (i == SRATE)
            {
                UTEST_ASSERT(cf.toggle());
            }
            else if ((i > SRATE) && (i < (SRATE * 2)))
            {
                UTEST_ASSERT(!cf.toggle());
            }

            // Perform cross-fade
            cf.process(dst, out, in, BUF_STEP);

            dst += BUF_STEP;
            if (out != NULL) out += BUF_STEP;
            if (in != NULL) in += BUF_STEP;
        }
    }

    UTEST_MAIN
    {
        float dst[BUF_SIZE], in[BUF_SIZE], out[BUF_SIZE];

        Crossfade cf;
        cf.init(SRATE * 2, 0.5); // 0.5 sec = SRATE samples

        // Initialize buffers
        dsp::fill(in, 0.5f, BUF_SIZE);
        dsp::fill(out, 1.0f, BUF_SIZE);
        process(cf, dst, out, in);
        UTEST_ASSERT(float_equals_absolute(dst[0], 1.0f));
        UTEST_ASSERT(float_equals_absolute(dst[BUF_SIZE - 1], 0.5f));
        UTEST_ASSERT(float_equals_absolute(dst[BUF_SIZE >> 1], 0.75f));

        // Initialize buffers
        dsp::fill(in, 0.5f, BUF_SIZE);
        process(cf, dst, NULL, in);
        UTEST_ASSERT(float_equals_absolute(dst[0], 0.0f));
        UTEST_ASSERT(float_equals_absolute(dst[BUF_SIZE - 1], 0.5f));
        UTEST_ASSERT(float_equals_absolute(dst[BUF_SIZE >> 1], 0.25f));

        // Initialize buffers
        dsp::fill(out, 0.5f, BUF_SIZE);
        process(cf, dst, out, NULL);
        UTEST_ASSERT(float_equals_absolute(dst[0], 0.5f));
        UTEST_ASSERT(float_equals_absolute(dst[BUF_SIZE - 1], 0.0f));
        UTEST_ASSERT(float_equals_absolute(dst[BUF_SIZE >> 1], 0.25f));

        // Initialize buffers
        process(cf, dst, NULL, NULL);
        UTEST_ASSERT(float_equals_absolute(dst[0], 0.0f));
        UTEST_ASSERT(float_equals_absolute(dst[BUF_SIZE - 1], 0.0f));
        UTEST_ASSERT(float_equals_absolute(dst[BUF_SIZE >> 1], 0.0f));
    }

UTEST_END

