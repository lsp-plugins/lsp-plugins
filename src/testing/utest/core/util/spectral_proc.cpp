/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 2 июл. 2020 г.
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
#include <core/util/SpectralProcessor.h>

using namespace lsp;

#define SRATE           48000.0f
#define TEST_FREQ       440.0f
#define SAMPLES         8192

UTEST_BEGIN("core.util", spectral_proc)

    void test_simple()
    {
        FloatBuffer in(SAMPLES);
        FloatBuffer out(SAMPLES);

        // Generate input data
        float *src  = in.data();
        float w     = 2 * M_PI * TEST_FREQ / SRATE;
        for (size_t i=0; i<8192; ++i)
            src[i]      = sinf(w * i);

        // Generate output data
        out.fill_zero();

        // Create processor
        SpectralProcessor sp;
        sp.init(14);
        sp.set_phase(0.0f);
        sp.set_rank(8);

        float *dst  = out.data();
        sp.process(dst, src, SAMPLES);

        UTEST_ASSERT(in.valid());
        UTEST_ASSERT(out.valid());

        // Compare data
        size_t latency = sp.latency();
        for (size_t i=0; i<SAMPLES-latency; ++i)
            UTEST_ASSERT(float_equals_absolute(src[i], dst[latency+i], 1e-5f));
    }

    UTEST_MAIN
    {
        test_simple();
    }
UTEST_END;
