/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 3 дек. 2020 г.
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

#include <plugins/art_delay.h>

namespace lsp
{
    art_delay_base::art_delay_base(const plugin_metadata_t &mdata, bool stereo_in): plugin_t(mdata)
    {
        bStereo         = stereo_in;
        vBuffer         = NULL;
        vDelays         = NULL;
        pExecutor       = NULL;

        pIn[0]          = NULL;
        pIn[1]          = NULL;
        pOut[0]         = NULL;
        pOut[1]         = NULL;
        pMaxDelay       = NULL;
        pPan[0]         = NULL;
        pPan[1]         = NULL;
        pDryGain        = NULL;
        pWetGain        = NULL;
        pMono           = NULL;
        pFeedback       = NULL;

        pData           = NULL;
    }

    art_delay_base::~art_delay_base()
    {
        destroy();
    }

    void art_delay_base::init(IWrapper *wrapper)
    {
    }

    void art_delay_base::destroy()
    {
        plugin_t::destroy();
    }

    bool art_delay_base::set_position(const position_t *pos)
    {
        return false;
    }

    void art_delay_base::update_settings()
    {
    }

    void art_delay_base::update_sample_rate(long sr)
    {
    }

    void art_delay_base::process(size_t samples)
    {
    }

    void art_delay_base::dump(IStateDumper *v) const
    {
    }

    art_delay_mono::art_delay_mono(): art_delay_base(metadata, false)
    {
    }

    art_delay_stereo::art_delay_stereo(): art_delay_base(metadata, true)
    {
    }
}



