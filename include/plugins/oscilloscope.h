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
#include <core/filters/FilterBank.h>

namespace lsp
{
    class oscilloscope_base: public plugin_t
    {
        protected:

            enum ch_update_t
            {
                UPD_SCPMODE             = 1 << 0,

                UPD_ACBLOCK_X           = 1 << 1,
                UPD_ACBLOCK_Y           = 1 << 2,
                UPD_ACBLOCK_EXT         = 1 << 3,

                UPD_OVERSAMPLER_X       = 1 << 4,
                UPD_OVERSAMPLER_Y       = 1 << 5,
                UPD_OVERSAMPLER_EXT     = 1 << 6,

                UPD_XY_RECORD_TIME      = 1 << 7,

                UPD_HOR_SCALES          = 1 << 8,

                UPD_PRETRG_DELAY        = 1 << 9,

                UPD_SWEEP_GENERATOR     = 1 << 10,

                UPD_VER_SCALES          = 1 << 11,

                UPD_TRIGGER_INPUT       = 1 << 12,
                UPD_TRIGGER_HOLD        = 1 << 13,
                UPD_TRIGGER             = 1 << 14,
                UPD_TRGGER_RESET        = 1 << 15
            };

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

            typedef struct dc_block_t
            {
                float   fAlpha;
                float   fGain;
            } dc_block_t;

            typedef struct ch_state_stage_t
            {
                size_t  nPV_pScpMode;

                size_t  nPV_pCoupling_x;
                size_t  nPV_pCoupling_y;
                size_t  nPV_pCoupling_ext;
                size_t  nPV_pOvsMode;

                size_t  nPV_pTrgInput;
                float   fPV_pVerDiv;
                float   fPV_pVerPos;
                float   fPV_pTrgLevel;
                float   fPV_pTrgHys;
                size_t  nPV_pTrgMode;
                float   fPV_pTrgHold;
                size_t  nPV_pTrgType;

                float   fPV_pTimeDiv;
                float   fPV_pHorDiv;
                float   fPV_pHorPos;

                size_t  nPV_pSweepType;

                float   fPV_pXYRecordTime;
            } ch_state_stage_t;

            typedef struct channel_t
            {
                ch_mode_t           enMode;
                ch_sweep_type_t     enSweepType;
                ch_trg_input_t      enTrgInput;
                ch_coupling_t       enCoupling_x;
                ch_coupling_t       enCoupling_y;
                ch_coupling_t       enCoupling_ext;

                FilterBank          sDCBlockBank_x;
                FilterBank          sDCBlockBank_y;
                FilterBank          sDCBlockBank_ext;

                over_mode_t         enOverMode;
                size_t              nOversampling;
                size_t              nOverSampleRate;

                Oversampler         sOversampler_x;
                Oversampler         sOversampler_y;
                Oversampler         sOversampler_ext;

                Delay               sPreTrgDelay;

                Trigger             sTrigger;

                Oscillator          sSweepGenerator;

                float              *vTemp;
                float              *vData_x;
                float              *vData_y;
                float              *vData_ext;
                float              *vData_y_delay;
                float              *vDisplay_x;
                float              *vDisplay_y;
                float              *vDisplay_s; // Strobe

                size_t              nDataHead;
                size_t              nDisplayHead;
                size_t              nSamplesCounter;
                bool                bClearStream;

                size_t              nPreTrigger;
                size_t              nSweepSize;

                float               fVerStreamScale;
                float               fVerStreamOffset;

                size_t              nXYRecordSize;
                float               fHorStreamScale;
                float               fHorStreamOffset;

                bool                bAutoSweep;
                size_t              nAutoSweepLimit;
                size_t              nAutoSweepCounter;

                ch_state_t          enState;

                size_t              nUpdate;
                ch_state_stage_t    sStateStage;
                bool                bUseGlobal;
                bool                bFreeze;

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
                IPort              *pCoupling_x;
                IPort              *pCoupling_y;
                IPort              *pCoupling_ext;

                IPort              *pSweepType;
                IPort              *pTimeDiv;
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

                IPort              *pGlobalSwitch;
                IPort              *pFreezeSwitch;
                IPort              *pSoloSwitch;
                IPort              *pMuteSwitch;

                IPort              *pStream;
            } channel_t;

        protected:
            dc_block_t  sDCBlockParams;
            size_t      nChannels;
            channel_t  *vChannels;

            size_t      nSampleRate;

            uint8_t    *pData;

            // Common Controls
            IPort      *pStrobeHistSize;
            IPort      *pXYRecordTime;
            IPort      *pFreeze;

            // Channel Selector
            IPort      *pChannelSelector;

            // Global ports:
            IPort      *pOvsMode;
            IPort      *pScpMode;
            IPort      *pCoupling_x;
            IPort      *pCoupling_y;
            IPort      *pCoupling_ext;

            IPort      *pSweepType;
            IPort      *pTimeDiv;
            IPort      *pHorDiv;
            IPort      *pHorPos;

            IPort      *pVerDiv;
            IPort      *pVerPos;

            IPort      *pTrgHys;
            IPort      *pTrgLev;
            IPort      *pTrgHold;
            IPort      *pTrgMode;
            IPort      *pTrgType;
            IPort      *pTrgInput;
            IPort      *pTrgReset;

        protected:
            over_mode_t get_oversampler_mode(size_t portValue);
            ch_mode_t get_scope_mode(size_t portValue);
            ch_sweep_type_t get_sweep_type(size_t portValue);
            ch_trg_input_t get_trigger_input(size_t portValue);
            ch_coupling_t get_coupling_type(size_t portValue);
            trg_mode_t get_trigger_mode(size_t portValue);
            trg_type_t get_trigger_type(size_t portValue);

        protected:
            void update_dc_block_filter(FilterBank &rFilterBank);
            void reconfigure_dc_block_filters();
            void do_sweep_step(channel_t *c, float strobe_value);
            float *select_trigger_input(float *extPtr, float* yPtr, ch_trg_input_t input);
            inline void set_oversampler(Oversampler &over, over_mode_t mode);
            inline void set_sweep_generator(channel_t *c);
            inline void configure_oversamplers(channel_t *c, over_mode_t mode);
            void init_state_stage(channel_t *c);
            void commit_staged_state_change(channel_t *c);
            void graph_stream(channel_t *c);

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

    class oscilloscope_x4: public oscilloscope_base, public oscilloscope_x4_metadata
    {
        public:
            oscilloscope_x4();
            virtual ~oscilloscope_x4();
    };
}

#endif /* PLUGINS_OSCILLOSCOPE_H_ */
