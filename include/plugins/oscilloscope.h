/*
 * oscilloscope.h
 *
 *  Created on: 27 Feb 2020
 *      Author: crocoduck
 */

#ifndef PLUGINS_OSCILLOSCOPE_H_
#define PLUGINS_OSCILLOSCOPE_H_

#include <core/plugin.h>
#include <metadata/plugins.h>
#include <core/util/Bypass.h>
#include <core/util/Delay.h>
#include <core/util/Oversampler.h>
#include <core/util/Trigger.h>
#include <core/util/Oscillator.h>

namespace lsp
{
    class oscilloscope_base: public plugin_t
    {
        protected:

            enum ch_mode_t
            {
                CH_MODE_XY,
                CH_MODE_TRIGGERED,

                CH_MODE_DFL = CH_MODE_TRIGGERED
            };

            enum ch_sweep_type_t
            {
                CH_SWEEP_TYPE_SAWTOOTH,
                CH_SWEEP_TYPE_TRIANGULAR,
                CH_SWEEP_TYPE_SINE,

                CH_SWEEP_TYPE_DFL = CH_SWEEP_TYPE_SAWTOOTH
            };

            enum ch_trg_input_t
            {
                CH_TRG_INPUT_Y,
                CH_TRG_INPUT_EXT,

                CH_TRG_INPUT_DFL = CH_TRG_INPUT_Y
            };

            enum ch_coupling_t
            {
                CH_COUPLING_AC,
                CH_COUPLING_DC,

                CH_COUPLING_DFL = CH_COUPLING_DC
            };

            enum ch_state_t
            {
                CH_STATE_LISTENING,
                CH_STATE_SWEEPING
            };

            typedef struct channel_t
            {
                ch_mode_t           enMode;
                ch_sweep_type_t     enSweepType;
                ch_trg_input_t      enTrgInput;
                ch_coupling_t       enCoupling;

                over_mode_t         enOverMode;
                size_t              nOversampling;
                size_t              nOverSampleRate;

                Oversampler         sOversampler_x;
                Oversampler         sOversampler_y;
                Oversampler         sOversampler_ext;

                Delay               sPreTrgDelay;

                Trigger             sTrigger;

                Oscillator          sSweepGenerator;

                float              *vData_x;
                float              *vData_y;
                float              *vData_ext;
                float              *vData_y_delay;
                float              *vDisplay_x;
                float              *vDisplay_y;

                size_t              nDataHead;
                size_t              nDisplayHead;
                size_t              nSamplesCounter;

                size_t              nPreTrigger;
                size_t              nSweepSize;

                float               fScale;
                float               fOffset;

                ch_state_t          enState;

                float              *vIn_x;
                float              *vIn_y;
                float              *vIn_ext;

                float              *vOut_x;
                float              *vOut_y;

                IPort              *pIn_x;
                IPort              *pIn_y;
                IPort              *pIn_ext;

                IPort              *pOut_x;
                IPort              *pOut_y;

                IPort              *pOvsMode;
                IPort              *pScpMode;
                IPort              *pCoupling;

                IPort              *pSweepType;
                IPort              *pHorDiv;
                IPort              *pHorPos;

                IPort              *pVerDiv;
                IPort              *pVerPos;

                IPort              *pTrgHys;
                IPort              *pTrgLev;
                IPort              *pTrgHold;
                IPort              *pTrgMode;
                IPort              *pTrgType;
                IPort              *pTrgInput;
                IPort              *pTrgReset;

                IPort              *pMesh;
            } channel_t;

        protected:
            size_t      nChannels;
            channel_t  *vChannels;

            size_t      nSampleRate;

            uint8_t    *pData;

        protected:
            over_mode_t get_oversampler_mode(size_t portValue);
            ch_mode_t get_scope_mode(size_t portValue);
            ch_sweep_type_t get_sweep_type(size_t portValue);
            ch_trg_input_t get_trigger_input(size_t portValue);
            ch_coupling_t get_coupling_type(size_t portValue);
            trg_mode_t get_trigger_mode(size_t portValue);
            trg_type_t get_trigger_type(size_t portValue);

        protected:
            void reset_display_buffers(channel_t *c);
            float *select_trigger_input(float *extPtr, float* yPtr, ch_trg_input_t input);
            inline void set_oversampler(Oversampler &over, over_mode_t mode);
            inline void set_sweep_generator(channel_t *c);
            inline void configure_oversamplers(channel_t *c);

        public:
            explicit oscilloscope_base(const plugin_metadata_t &metadata, size_t channels);
            virtual ~oscilloscope_base();

        public:
            virtual void init(IWrapper *wrapper);
            virtual void destroy();

            virtual void update_settings();
            virtual void update_sample_rate(long sr);

            virtual void process(size_t samples);

            virtual void dump(IStateDumper *v) const;
    };

    class oscilloscope_x1: public oscilloscope_base, public oscilloscope_x1_metadata
    {
        public:
            oscilloscope_x1();
            virtual ~oscilloscope_x1();
    };

    class oscilloscope_x2: public oscilloscope_base, public oscilloscope_x2_metadata
    {
        public:
            oscilloscope_x2();
            virtual ~oscilloscope_x2();
    };
}

#endif /* PLUGINS_OSCILLOSCOPE_H_ */
