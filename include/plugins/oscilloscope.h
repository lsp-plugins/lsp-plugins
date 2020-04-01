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
#include <core/util/ShiftBuffer.h>
#include <core/util/Oversampler.h>
#include <core/util/Trigger.h>

namespace lsp
{
    class oscilloscope_base: public plugin_t
    {
        protected:
            enum channel_state_t
            {
                LISTENING,
                SWEEPING
            };

            typedef struct channel_t
            {
                Bypass          sBypass;
                Oversampler     sOversampler;
                ShiftBuffer     sShiftBuffer;
                Trigger         sTrigger;

                over_mode_t     enOverMode;
                size_t          nOversampling;
                size_t          nOverSampleRate;

                size_t          nSamplesCounter;
                size_t          nBufferScanningHead;
                size_t          nBufferCopyHead;
                size_t          nBufferCopyCount;

                bool            bProcessComplete;

                size_t          nPreTrigger;
                size_t          nPostTrigger;
                size_t          nSweepSize;
                size_t          nSweepHead;

                bool            bDoPlot;

                float          *vAbscissa;
                float          *vOrdinate;

                float          *vSweep;

                channel_state_t enState;

                float          *vIn;
                float          *vOut;

                IPort          *pIn;
                IPort          *pOut;

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

            size_t      nCaptureSize;

            size_t      nMeshSize;

            float      *vTemp;

            float      *vDflAbscissa;

            IPort      *pBypass;

            uint8_t    *pData;

        protected:
            void get_plottable_data(float *dst, float *src, size_t dstCount, size_t srcCount);

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
