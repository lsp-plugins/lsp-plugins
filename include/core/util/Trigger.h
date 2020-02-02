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

            trg_type_t      enTriggerType;
            trg_state_t     enTriggerState;

            size_t          nPostTrigger;
            size_t          nPostTriggerCounter;

            float           fThreshold;

            float          *vMemory;
            uint8_t        *pData;

            bool            bSync;

        public:
            Trigger();
            ~Trigger();

        protected:

            void prepare_memory();

            void shift_memory();

        public:

            bool init();

            void destroy();

            inline bool needs_update() const
            {
                return bSync;
            }

            void update_settings();

            inline void set_post_trigger_samples(size_t nSamples)
            {
                if (nSamples == nPostTrigger)
                    return;

                nPostTrigger = nSamples;
            }

            inline void set_trigger_type(trg_type_t type)
            {
                if ((type <= TRG_TYPE_NONE) || (type >= TRG_TYPE_MAX) || (enTriggerType == type))
                    return;

                enTriggerType = type;
                bSync = true;
            }

            void single_sample_processor(float value);
    };
}

#endif /* CORE_UTIL_TRIGGER_H_ */
