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
//#include <core/util/ShiftBuffer.h>
#include <core/util/Delay.h>
#include <core/util/Oversampler.h>
#include <core/util/Trigger.h>
#include <core/util/SweepGenerator.h>

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

            enum ch_output_mode_t
            {
                CH_OUTPUT_MODE_MUTE,
                CH_OUTPUT_MODE_COPY,
                CH_OUTPUT_MODE_DFL = CH_OUTPUT_MODE_COPY
            };

            enum ch_trg_input_t
            {
                CH_TRG_INPUT_EXT,
                CH_TRG_INPUT_Y,
                CH_TRG_INPUT_DFL = CH_TRG_INPUT_Y
            };

            enum ch_state_t
            {
                CH_STATE_LISTENING,
                CH_STATE_SWEEPING
            };

            typedef struct channel_t
            {
                ch_mode_t           enMode;
                ch_output_mode_t    enOutputMode;
                ch_trg_input_t      enTrgInput;

                Bypass              sBypass;
//                ShiftBuffer     sShiftBuffer;

                over_mode_t         enOverMode;
                size_t              nOversampling;
                size_t              nOverSampleRate;

                Oversampler         sOversampler_x;
                Oversampler         sOversampler_y;
                Oversampler         sOversampler_ext;

                Delay               sPreTrgDelay;

                Trigger             sTrigger;

                SweepGenerator      sSweepGenerator;

                float              *vData_x;
                float              *vData_y;
                float              *vData_ext;
                float              *vData_y_delay;
                float              *vDisplay_x;
                float              *vDisplay_y;

                size_t              nDisplayHead;
                size_t              nSamplesCounter;

                size_t              nPreTrigger;
                size_t              nSweepSize;

                float               fScale;
                float               fOffset;

//                size_t          nSamplesCounter;
//                size_t          nBufferScanningHead;
//                size_t          nBufferCopyHead;
//                size_t          nBufferCopyCount;
//
//                bool            bProcessComplete;
//
//                size_t          nPreTrigger;
//                size_t          nPostTrigger;
//                size_t          nSweepSize;
//                size_t          nSweepHead;
//
//                float           fScale;
//                float           fOffset;
//
//                bool            bDoPlot;
//
//                float          *vAbscissa;
//                float          *vOrdinate;
//
//                float          *vSweep;
//
//                float          *vOutput;

                ch_state_t      enState;

                float          *vIn_x;
                float          *vIn_y;
                float          *vIn_ext;

                float          *vOut_x;
                float          *vOut_y;

                IPort          *pIn_x;
                IPort          *pIn_y;
                IPort          *pIn_ext;

                IPort          *pOut_x;
                IPort          *pOut_y;

                IPort          *pHorDiv;
                IPort          *pHorPos;

                IPort          *pVerDiv;
                IPort          *pVerPos;

                IPort          *pTrgHys;
                IPort          *pTrgLev;
                IPort          *pTrgMode;
                IPort          *pTrgType;

                IPort          *pCoupling;

                IPort          *pMesh;
            } channel_t;

        protected:
            size_t      nChannels;
            channel_t  *vChannels;

            size_t      nSampleRate;

//            size_t      nCaptureSize;
//
//            size_t      nMeshSize;
//
//            float      *vTemp;
//
//            float      *vDflAbscissa;

            IPort      *pBypass;

            uint8_t    *pData;

        protected:
            void calculate_output(float *dst, float *src, size_t count, ch_output_mode_t mode);
            bool fill_display_buffers(channel_t *c, float *xBuf, float *yBuf, size_t bufSize);
            void reset_display_buffers(channel_t *c);
            float *select_trigger_input(float *extPtr, float* yPtr, ch_trg_input_t input);
//            void route_display_data(float *raw_data, float *proc_data, float *display, size_t count, ch_mode_t mode);
            inline void set_oversampler(Oversampler &over, over_mode_t mode);
//            void get_plottable_data(float *dst, float *src, size_t dstCount, size_t srcCount);

        public:
            oscilloscope_base(const plugin_metadata_t &metadata, size_t channels);
            virtual ~oscilloscope_base();

        public:
            virtual void init(IWrapper *wrapper);
            virtual void destroy();

            virtual void update_settings();
            virtual void update_sample_rate(long sr);

            virtual void process(size_t samples);
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
