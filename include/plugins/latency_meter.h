/*
 * latency_meter.h
 *
 *  Created on: 2 May 2017
 *      Author: crocoduck
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
