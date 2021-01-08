/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 26 дек. 2020 г.
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

#include <test/mtest.h>
#include <test/helpers.h>
#include <core/files/AudioFile.h>
#include <core/util/Analyzer.h>
#include <test/FloatBuffer.h>
#include <dsp/dsp.h>

using namespace lsp;

MTEST_BEGIN("core.util", analyzer)

    MTEST_MAIN
    {
        Analyzer spa;

        spa.init(2, 14, 192000, 20.0f);
        spa.set_activity(true);
        spa.set_envelope(envelope::PINK_NOISE);
        spa.set_window(windows::BLACKMAN_HARRIS);
        spa.set_sample_rate(48000);
        spa.set_rate(40.0f);
        spa.set_rank(12);
        spa.set_shift(1.0f);

        spa.enable_channel(0, true);
        spa.enable_channel(1, true);

        FloatBuffer in(1024);
        dsp::fill_zero(in, in.size());
        in[0] = 1.0f;

        const float *vbuf[2];
        vbuf[0] = in;
        vbuf[1] = in;

        for (size_t i=0; i<10; ++i)
        {
            printf("process %d\n", int(i));
            for (size_t j=0; j<spa.get_channels(); ++j)
                spa.process(vbuf, in.size());

            dsp::fill_zero(in, in.size());
        }
    }

MTEST_END




