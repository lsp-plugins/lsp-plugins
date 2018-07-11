/*
 * latency_meter.h
 *
 *  Created on: 2 May 2017
 *      Author: crocoduck
 */

#ifndef METADATA_LATENCY_METER_H_
#define METADATA_LATENCY_METER_H_

namespace lsp
{
    struct latency_meter_metadata
    {
            static const plugin_metadata_t metadata;

            static const float LATENCY_MIN          = 0.0f;       /* Min detectable latency [ms] */
            static const float LATENCY_MAX          = 2000.0f;    /* Max detectable latency [ms] */
            static const float LATENCY_DFL          = 1000.0f;
            static const float LATENCY_STEP         = 1.0f;

            static const float MTR_LATENCY_MIN      = 0.0f;       /* Min detectable latency [ms] */
            static const float MTR_LATENCY_MAX      = 2000.0f;    /* Max detectable latency [ms] */
            static const float MTR_LATENCY_DFL      = 0.0f;
            static const float MTR_LATENCY_STEP     = 1.0f;

            static const float PEAK_THRESHOLD_MIN   = GAIN_AMP_M_84_DB;
            static const float PEAK_THRESHOLD_MAX   = GAIN_AMP_0_DB;
            static const float PEAK_THRESHOLD_DFL   = GAIN_AMP_M_24_DB;
            static const float PEAK_THRESHOLD_STEP  = 0.01f;

            static const float ABS_THRESHOLD_MIN    = GAIN_AMP_M_84_DB;
            static const float ABS_THRESHOLD_MAX    = GAIN_AMP_0_DB;
            static const float ABS_THRESHOLD_DFL    = GAIN_AMP_M_24_DB;
            static const float ABS_THRESHOLD_STEP   = 0.01f;
    };
}

#endif /* METADATA_LATENCY_METER_H_ */
