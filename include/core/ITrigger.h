/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 08 апр. 2016 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
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
