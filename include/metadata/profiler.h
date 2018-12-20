/*
 * profiler.h
 *
 *  Created on: 12 Sep 2017
 *      Author: crocoduck
 */

#ifndef METADATA_PROFILER_H_
#define METADATA_PROFILER_H_

namespace lsp
{
    struct profiler_base_metadata
    {
            static const plugin_metadata_t metadata;

            static const float FREQUENCY_MIN        = 20.0f;
            static const float FREQUENCY_MAX        = 20000.0f;
            static const float FREQUENCY_DFL        = 1000.0f;
            static const float FREQUENCY_STEP       = 0.01f;

            static const float AMPLITUDE_DFL        = 1.0f;

            static const float LATENCY_MIN          = 0.0f;         /* Min detectable latency [ms] */
            static const float LATENCY_MAX          = 2000.0f;      /* Max detectable latency [ms] */
            static const float LATENCY_DFL          = 1000.0f;
            static const float LATENCY_STEP         = 1.0f;

            static const float PEAK_THRESHOLD_MIN   = GAIN_AMP_M_84_DB;
            static const float PEAK_THRESHOLD_MAX   = GAIN_AMP_0_DB;
            static const float PEAK_THRESHOLD_DFL   = GAIN_AMP_M_24_DB;
            static const float PEAK_THRESHOLD_STEP  = 0.01f;

            static const float ABS_THRESHOLD_MIN    = GAIN_AMP_M_84_DB;
            static const float ABS_THRESHOLD_MAX    = GAIN_AMP_0_DB;
            static const float ABS_THRESHOLD_DFL    = GAIN_AMP_M_24_DB;
            static const float ABS_THRESHOLD_STEP   = 0.01f;

            static const float DURATION_MIN         = 1.0f;
            static const float DURATION_MAX         = 50.0f;
            static const float DURATION_DFL         = 10.0f;
            static const float DURATION_STEP        = 0.5f;

            static const float MTR_T_MIN            = 0.0f;
            static const float MTR_T_MAX            = 60.0f;
            static const float MTR_T_DFL            = 0.0f;
            static const float MTR_T_STEP           = 1.0f;

            static const float IR_OFFSET_MIN        = -1000.0f;
            static const float IR_OFFSET_MAX        = 1000.0f;
            static const float IR_OFFSET_DFL        = 0.0f;
            static const float IR_OFFSET_STEP       = 0.01f;

            static const float MTR_LATENCY_MIN      = 0.0f;         /* Min detectable latency [ms] */
            static const float MTR_LATENCY_MAX      = 2000.0f;      /* Max detectable latency [ms] */
            static const float MTR_LATENCY_DFL      = 0.0f;
            static const float MTR_LATENCY_STEP     = 1.0f;

            enum rt_algorithm_selector_t
            {
                SC_RTALGO_EDT_0,
                SC_RTALGO_EDT_1,
                SC_RTALGO_T_10,
                SC_RTALGO_T_20,
                SC_RTALGO_T_30,

                SC_RTALGO_DFL = SC_RTALGO_T_20
            };

            static const float MTR_RT_MIN           = 0.0f;
            static const float MTR_RT_MAX           = 60.0f;
            static const float MTR_RT_DFL           = 0.0f;
            static const float MTR_RT_STEP          = 1.0f;

            static const float MTR_IL_MIN           = 0.0f;
            static const float MTR_IL_MAX           = 60.0f;
            static const float MTR_IL_DFL           = 0.0f;
            static const float MTR_IL_STEP          = 1.0f;

            static const float MTR_R_MIN            = -1.0f;
            static const float MTR_R_MAX            = 1.0f;
            static const float MTR_R_DFL            = 0.0f;
            static const float MTR_R_STEP           = 0.001f;

            enum saving_mode_selector_t
            {
                SC_SVMODE_AUTO,
                SC_SVMODE_RT,
                SC_SVMODE_IT,
                SC_SVMODE_ALL,
                SC_SVMOD_NLINEAR,

                SC_SVMODE_DFL = SC_SVMODE_AUTO
            };

            static const size_t RESULT_MESH_SIZE    = 512;
    };

    struct profiler_mono_metadata: public profiler_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct profiler_stereo_metadata: public profiler_base_metadata
    {
        static const plugin_metadata_t metadata;
    };
}

#endif /* METADATA_PROFILER_H_ */
