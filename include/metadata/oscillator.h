/*
 * oscillator.h
 *
 *  Created on: 20 Mar 2017
 *      Author: crocoduck
 */

#ifndef METADATA_OSCILLATOR_H_
#define METADATA_OSCILLATOR_H_

namespace lsp
{
    struct oscillator_mono_metadata
    {
            static const plugin_metadata_t metadata;

            static const float FREQUENCY_MIN        = 20.0f;
            static const float FREQUENCY_MAX        = 20000.0f;
            static const float FREQUENCY_DFL        = 440.0f;
            static const float FREQUENCY_STEP       = 0.001f;

            static const float DCOFFSET_MIN         = -1.0f;
            static const float DCOFFSET_MAX         = 1.0f;
            static const float DCOFFSET_DFL         = 0.0f;
            static const float DCOFFSET_STEP        = 0.001f;

            static const float INITPHASE_MIN        = 0.0f;
            static const float INITPHASE_MAX        = 360.0f;
            static const float INITPHASE_DFL        = 0.0f;
            static const float INITPHASE_STEP       = 0.1f;

            static const size_t HISTORY_MESH_SIZE   = 280;

            enum function_selector_t
            {
                SC_FUNC_SINE,
                SC_FUNC_COSINE,
                SC_FUNC_SQUARED_SINE,
                SC_FUNC_SQUARED_COSINE,
                SC_FUNC_RECTANGULAR,
                SC_FUNC_SAWTOOTH,
                SC_FUNC_TRAPEZOID,
                SC_FUNC_PULSETRAIN,
                SC_FUNC_PARABOLIC,
                SC_FUNC_BL_RECTANGULAR,
                SC_FUNC_BL_SAWTOOTH,
                SC_FUNC_BL_TRAPEZOID,
                SC_FUNC_BL_PULSETRAIN,
                SC_FUNC_BL_PARABOLIC,

                SC_FUNC_DFL = SC_FUNC_SINE
            };

            enum dc_reference_selector_t
            {
                SC_DC_WAVEDC,
                SC_DC_ZERO,

                SC_DC_DFL = SC_DC_WAVEDC
            };

            enum operation_mode_selector_t
            {
                SC_MODE_ADD,
                SC_MODE_MUL,
                SC_MODE_REP,

                SC_MODE_DFL = SC_MODE_ADD
            };

            enum oversampler_mode_selector_t
            {
                SC_OVS_NONE,
                SC_OVS_2X,
                SC_OVS_3X,
                SC_OVS_4X,
                SC_OVS_6X,
                SC_OVS_8X,

                SC_OVS_DFL = SC_OVS_8X
            };

    };
}

#endif /* METADATA_OSCILLATOR_H_ */
