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
    };

    class Oscilloscope
    {
        private:
            Oscilloscope & operator = (const Oscilloscope &);

        protected:

        typedef struct sweep_t
        {
            float   fPreTrigger;
            float   fPostTrigger;

            size_t  nPreTrigger;
            size_t  nPostTrigger;

            float   fVertOffset;
            float   fHorOffset;

            size_t  nSweepLength;

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
            size_t              nOverSampleRate;

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

            explicit Oscilloscope();
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

            /** Set the output mode of the oscilloscope.
             *
             * @param mode oscilloscope output mode.
             */
            inline void set_output_mode(osc_output_mode_t mode)
            {
                if ((mode == enOutputMode) || (mode <= OSC_OUTPUT_MODE_MUTED) || (mode >= OSC_OUTPUT_MODE_MAX))
                    return;

                enOutputMode = mode;
            }

            /** Set the pre-trigger time that will be included in the sweeping buffer.
             *
             * @param preTriggerTime pre-trigger time, in seconds.
             */
            inline void set_pre_trigger_time(float preTriggerTime)
            {
                if ((preTriggerTime == sSweepParams.fPreTrigger) || (preTriggerTime <= 0.0f))
                    return;

                sSweepParams.fPreTrigger = preTriggerTime;
                bSync = true;
            }

            /** Set the post-trigger time that will be included in the sweeping buffer.
             *
             * @param postTriggerTime post-trigger time, in seconds.
             */
            inline void set_post_trigger_time(float posTriggerTime)
            {
                if ((posTriggerTime == sSweepParams.fPostTrigger) || (posTriggerTime <= 0.0f))
                    return;

                sSweepParams.fPostTrigger = posTriggerTime;
                bSync = true;
            }

            /** Set symmetric pre and post trigger times.
             *
             * @param sweepTime sweep time that will be assigned as pre and post time, seconds.
             */
            inline void set_symmetric_sweep_times(float sweepTime)
            {
                if (((sweepTime == sSweepParams.fPreTrigger) && (sweepTime == sSweepParams.fPostTrigger)) || (sweepTime <= 0.0f))
                    return;

                sSweepParams.fPreTrigger = sweepTime;
                sSweepParams.fPostTrigger = sweepTime;
                bSync = true;
            }

            /** Set trigger type.
             *
             * @param type trigger type.
             */
            inline void set_trigger_type(trg_type_t type)
            {
                if (type == enTriggerType)
                    return;

                enTriggerType = type;
                bSync = true;
            }

            /** Get the length of the sweep.
             *
             * @return sweep length in samples.
             */
            inline size_t get_sweep_length()
            {
                return sSweepParams.nSweepLength;
            }

            void process(float *dst, float *src, size_t count);

            /** If the sweep is complete, return true.
             *
             * @return true if the sweep is complete.
             */
            inline bool get_sweep_complete()
            {
                return sSweepParams.bSweepComplete;
            }

            /** Get a pointer to the internal sweep buffer.
             *
             * @return pointer to the internal sweep buffer.
             */
            inline float * get_sweep_buffer()
            {
                return vSweepBuffer;
            }

            /** Get the sweep buffer length.
             *
             * @return sweep buffer length.
             */
            inline size_t get_sweep_buffer_length()
            {
                return sSweepParams.nSweepLength;
            }
    };
}

#endif /* CORE_UTIL_OSCILLOSCOPE_H_ */
