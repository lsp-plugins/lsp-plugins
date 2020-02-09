/*
 * Oscilloscope.h
 *
 *  Created on: 27 Jan 2020
 *      Author: crocoduck
 */

#ifndef CORE_UTIL_OSCILLOSCOPE_H_
#define CORE_UTIL_OSCILLOSCOPE_H_

#include <core/types.h>
#include <core/util/Trigger.h>
#include <core/util/Oversampler.h>

namespace lsp
{

    enum osc_output_mode_t
    {
        OSC_OUTPUT_MODE_MUTED,
        OSC_OUTPUT_MODE_COPY,
        OSC_OUTPUT_MODE_MAX,

        OSC_OUTPUT_MODE_DFL = OSC_OUTPUT_MODE_MUTED
    };

    enum osc_state_t
    {
        OSC_STATE_ACQUIRING,
        OSC_STATE_SWEEPING,
        OSC_STATE_MAX
    };

    class Oscilloscope
    {

        protected:

        typedef struct sweep_t
        {
            float   fPreTrigger;
            float   fPostTrigger;

            size_t  nPreTrigger;
            size_t  nPostTrigger;

            float   fVertOffset;
            float   fHorOffset;

            size_t  nLimit;

            size_t  nHead;

            bool    bSweepComplete;
        } sweep_t;

        typedef struct buffer_t
        {
            size_t  nHead;
            size_t  nTriggerAt;
//            size_t  nRemaining;
        } buffer_t;

        private:

            size_t              nSampleRate;

            Trigger             sTrigger;
            trg_type_t          enTriggerType;

            osc_state_t         enState;

            sweep_t             sSweepParams;

            buffer_t            sBufferParams;

            Oversampler         sOver;
            over_mode_t         enOverMode;
            size_t              nOversampling;

            osc_output_mode_t   enOutputMode;

            float              *vCaptureBuffer;
            float              *vSweepBuffer;
            uint8_t            *pData;

            bool                bSync;

        public:

            Oscilloscope();
            ~Oscilloscope();

        protected:

            void sweep_from_the_past();

        public:

            /** Initialise Oscilloscope.
             *
             */
            bool init();

            /** Destroy Oscilloscope.
             *
             */
            void destroy();

            /** Check that oscilloscope needs settings update.
             *
             * @return true if oscilloscope needs settings update.
             */
            inline bool needs_update() const
            {
                return bSync;
            }

            /** This method should be called if needs_update() returns true.
             * before calling process() methods.
             *
             */
            void update_settings();

            /** Set sample rate for the function generator.
             *
             * @param sr sample rate.
             */
            inline void set_sample_rate(size_t sr)
            {
                if (nSampleRate == sr)
                    return;

                nSampleRate = sr;
                bSync       = true;
            }

            /** Set Oversampler mode.
             *
             * @param mode oversampler mode.
             */
            inline void set_oversampler_mode(over_mode_t mode)
            {
                if (mode == enOverMode)
                    return;

                enOverMode  = mode;
                bSync       = true;
            }

            inline void set_output_mode(osc_output_mode_t mode)
            {
                if ((mode == enOutputMode) || (mode <= OSC_OUTPUT_MODE_MUTED) || (mode >= OSC_OUTPUT_MODE_MAX))
                    return;

                enOutputMode = mode;
            }

            inline void set_pre_trigger_time(float preTriggerTime)
            {
                if ((preTriggerTime == sSweepParams.fPreTrigger) || (preTriggerTime <= 0.0f))
                    return;

                sSweepParams.fPreTrigger = preTriggerTime;
                bSync = true;
            }

            inline void set_post_trigger_time(float posTriggerTime)
            {
                if ((posTriggerTime == sSweepParams.fPostTrigger) || (posTriggerTime <= 0.0f))
                    return;

                sSweepParams.fPostTrigger = posTriggerTime;
                bSync = true;
            }

            inline void set_symmetric_sweep_times(float sweepTime)
            {
                if (((sweepTime == sSweepParams.fPreTrigger) && (sweepTime == sSweepParams.fPostTrigger)) || (sweepTime <= 0.0f))
                    return;

                sSweepParams.fPreTrigger = sweepTime;
                sSweepParams.fPostTrigger = sweepTime;
                bSync = true;
            }

            inline void set_trigger_type(trg_type_t type)
            {
                if (type == enTriggerType)
                    return;

                enTriggerType = type;
                bSync = true;
            }

            void process(float *dst, float *src, size_t count);

            inline bool get_sweep_complete()
            {
                return sSweepParams.bSweepComplete;
            }
    };
}

#endif /* CORE_UTIL_OSCILLOSCOPE_H_ */
