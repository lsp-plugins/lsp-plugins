/*
 * Trigger.h
 *
 *  Created on: 29 Jan 2020
 *      Author: crocoduck
 */

#ifndef CORE_UTIL_TRIGGER_H_
#define CORE_UTIL_TRIGGER_H_

#include <core/types.h>

namespace lsp
{

    enum trg_type_t
    {
        TRG_TYPE_NONE,
        TRG_TYPE_EXTERNAL,
        TRG_TYPE_SIMPLE_RISING_EDGE,
        TRG_TYPE_SIMPLE_FALLING_EDGE,
        TRG_TYPE_MAX
    };

    enum trg_state_t
    {
        TRG_STATE_WAITING,
        TRG_STATE_ARMED,
        TRG_STATE_FIRED,
        TRG_STATE_MAX
    };

    class Trigger
    {
        private:
            Trigger & operator = (const Trigger &);

        private:

            trg_type_t      enTriggerType;
            trg_state_t     enTriggerState;

            size_t          nPostTrigger;
            size_t          nPostTriggerCounter;

            float           fThreshold;

            size_t          nExternalTriggerCounter;

            size_t          nMemoryHead;

            float          *vMemory;
            uint8_t        *pData;

            bool            bSync;

        public:
            explicit Trigger();
            ~Trigger();

        protected:

            /** Prepare the internal data buffer.
             *
             */
            void prepare_memory();

        public:

            /** Initialise trigger.
             *
             */
            bool init();

            /** Destroy trigger.
             *
             */
            void destroy();

            /** Check that trigger needs settings update.
             *
             * @return true if trigger needs settings update.
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

            /** Set the post-trigger samples. The trigger can be allowed to fire only after the post-trigger samples have elapsed.
             *
             * @param nSamples number of post-trigger samples.
             */
            inline void set_post_trigger_samples(size_t nSamples)
            {
                if (nSamples == nPostTrigger)
                    return;

                nPostTrigger = nSamples;
            }

            /** Set the trigger type.
             *
             * @param type trigger type.
             */
            inline void set_trigger_type(trg_type_t type)
            {
                if ((type <= TRG_TYPE_NONE) || (type >= TRG_TYPE_MAX) || (enTriggerType == type))
                    return;

                enTriggerType = type;
                bSync = true;
            }

            /** Set the trigger threshold.
             *
             * @param trigger threshold.
             */
            inline void set_trigger_threshold(float threshold)
            {
                fThreshold = threshold;
            }

            /** Return he trigger state.
             *
             * @return trigger state.
             */
            inline trg_state_t get_trigger_state() const
            {
                return enTriggerState;
            }

            /** Arm the trigger. This method arms the trigger only if the state is TRG_TYPE_EXTERNAL, and does nothing otherwise.
             *
             * After this method is called, the external trigger will fire as soon as single_sample_processor method is called. Then, the state is reseted to TRG_STATE_WAITING.
             */
            inline void arm_external_trigger(size_t externalCounter)
            {
                if (enTriggerType != TRG_TYPE_EXTERNAL)
                    return;

                enTriggerState = TRG_STATE_ARMED;
                nExternalTriggerCounter = externalCounter;
            }

            /** Feed a single sample to the trigger. Query the trigger status afterwards.
             *
             */
            void single_sample_processor(float value);
    };
}

#endif /* CORE_UTIL_TRIGGER_H_ */
