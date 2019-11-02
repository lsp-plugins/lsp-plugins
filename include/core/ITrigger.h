/*
 * ITrigger.hpp
 *
 *  Created on: 08 апр. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_ITRIGGER_H_
#define CORE_ITRIGGER_H_

#include <core/types.h>

namespace lsp
{
    /** Trigger callback interface
     *
     */
    class ITrigger
    {
        private:
            ITrigger & operator = (const ITrigger &);

        public:
            explicit ITrigger();
            virtual ~ITrigger();

        public:
            /** Handle the trigger ON event
             *
             * @param timestamp timestamp of the event
             * @param level signal level
             */
            virtual void trigger_on(size_t timestamp, float level);

            /** Handle the trigger OFF event
             *
             * @param timestamp timestamp of the event
             * @param level signal level
             */
            virtual void trigger_off(size_t timestamp, float level);

            /** Handle the stop playback signal
             *
             * @param timestamp timestamp of the event
             */
            virtual void trigger_stop(size_t timestamp);
    };

} /* namespace lsp */

#endif /* CORE_ITRIGGER_H_ */
