/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 5 сент. 2020 г.
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
#include <core/util/Convolver.h>
#include <dsp/dsp.h>

using namespace lsp;

MTEST_BEGIN("core.util", convolver)

    MTEST_MAIN
    {
        Convolver cv;
        AudioFile vox, conv, out, dir;

        MTEST_ASSERT(vox.load("tmp/convolver/vox.wav") == STATUS_OK);
        MTEST_ASSERT(conv.load("tmp/convolver/mono-hall.wav") == STATUS_OK);

        MTEST_ASSERT(cv.init(conv.channel(0), conv.samples(), 13, 0.0f));

        MTEST_ASSERT(out.create_samples(1, vox.sample_rate(), vox.samples() + conv.samples()) == STATUS_OK);
        MTEST_ASSERT(dir.create_samples(1, vox.sample_rate(), vox.samples() + conv.samples()) == STATUS_OK);

        cv.process(out.channel(0), vox.channel(0), vox.samples());
        dsp::convolve(dir.channel(0), vox.channel(0), conv.channel(0), conv.samples(), vox.samples());

        MTEST_ASSERT(out.store("tmp/convolver/processed.wav") == STATUS_OK);
        MTEST_ASSERT(dir.store("tmp/convolver/direct.wav") == STATUS_OK);
    }

MTEST_END

