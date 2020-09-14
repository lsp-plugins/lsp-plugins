/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Stefano Tronci <stefano.tronci@protonmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 2 May 2017
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

#ifndef PLUGINS_LATENCY_METER_H_
#define PLUGINS_LATENCY_METER_H_

#include <core/plugin.h>
#include <metadata/plugins.h>
#include <core/util/Bypass.h>
#include <core/util/LatencyDetector.h>

namespace lsp
{
    class latency_meter: public plugin_t, public latency_meter_metadata
    {
        protected:
            LatencyDetector     sLatencyDetector;
            Bypass              sBypass;
            bool                bBypass;
            bool                bTrigger;
            bool                bFeedback;
            float               fInGain;
            float               fOutGain;

            float              *vBuffer;
            uint8_t            *pData;

            IPort              *pIn;
            IPort              *pOut;
            IPort              *pBypass;
            IPort              *pMaxLatency;
            IPort              *pPeakThreshold;
            IPort              *pAbsThreshold;
            IPort              *pInputGain;
            IPort              *pFeedback;
            IPort              *pOutputGain;
            IPort              *pTrigger;
            IPort              *pLatencyScreen;
            IPort              *pLevel;

        public:
            latency_meter();
            virtual ~latency_meter();

        public:
            virtual void init(IWrapper *wrapper);
            virtual void process(size_t samples);
            virtual void update_settings();
            virtual void update_sample_rate(long sr);
            virtual void destroy();

    };
}

#endif /* PLUGINS_LATENCY_METER_H_ */
