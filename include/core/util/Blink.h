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

#ifndef CORE_UTIL_BLINK_H_
#define CORE_UTIL_BLINK_H_

#include <core/types.h>
#include <core/IStateDumper.h>

namespace lsp
{
    /** Simple blink counter
     *
     */
    class Blink
    {
        private:
            Blink & operator = (const Blink &);

        protected:
            ssize_t     nCounter;
            ssize_t     nTime;
            float       fOnValue;
            float       fOffValue;
            float       fTime;

        public:
            explicit Blink();
            ~Blink();

            void construct();

        public:
            /** Initialize blink
             *
             * @param sample_rate sample rate
             * @param time activity time
             */
            inline void init(size_t sample_rate, float time = 0.1f)
            {
                nCounter        = 0;
                nTime           = seconds_to_samples(sample_rate, time);
                fTime           = time;
            }

            /** Update current sample rate
             *
             * @param sample_rate current sample rate
             */
            inline void set_sample_rate(size_t sample_rate)
            {
                nTime           = seconds_to_samples(sample_rate, fTime);
            }

            /** Make blinking
             *
             */
            inline void blink()
            {
                nCounter        = nTime;
                fOnValue        = 1.0f;
            }

            /** Make blinking
             * @param value value to display
             */
            inline void blink(float value)
            {
                nCounter        = nTime;
                fOnValue        = value;
            }

            /** Make blinking
             *
             * @param value value that will be displayed if less than max value
             */
            inline void blink_max(float value)
            {
                if ((nCounter <= 0) || (fOnValue < value))
                {
                    fOnValue        = value;
                    nCounter        = nTime;
                }
            }

            /** Make blinking
             *
             * @param value value that will be displayed if less than max value
             */
            inline void blink_min(float value)
            {
                if ((nCounter <= 0) || (fOnValue > value))
                {
                    fOnValue        = value;
                    nCounter        = nTime;
                }
            }

            /** Set default values
             *
             * @param on default value for on state
             * @param off default value for off state
             */
            inline void set_default(float on, float off)
            {
                fOffValue       = off;
                fOnValue        = on;
            }

            /** Set default value for off state
             *
             * @param off default value for off state
             */
            inline void set_default_off(float off)
            {
                fOffValue       = off;
            }

            /** Process blinking
             *
             * @return activity value
             */
            inline float process(size_t samples)
            {
                float result    = (nCounter > 0) ? fOnValue : fOffValue;
                nCounter       -= samples;
                return result;
            }

            /** Get current activity value of the blink
             *
             * @return current activity value of the blink
             */
            inline float value() const
            {
                return (nCounter > 0) ? fOnValue : fOffValue;
            }

            /**
             * Dump the state
             * @param dumper dumper
             */
            void dump(IStateDumper *v) const;
    };

} /* namespace lsp */

#endif /* CORE_UTIL_BLINK_H_ */
