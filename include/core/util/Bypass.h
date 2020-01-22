/*
 * Bypass.h
 *
 *  Created on: 07 дек. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_UTIL_BYPASS_H_
#define CORE_UTIL_BYPASS_H_

#include <dsp/dsp.h>

namespace lsp
{
    /**
     * Bypass class, provides utilitary class for implementing mono bypass function
     */
    class Bypass
    {
        private:
            Bypass & operator = (const Bypass &);

        private:
            enum state_t
            {
                S_ON,
                S_ACTIVE,
                S_OFF
            };

            state_t nState;
            float   fDelta;
            float   fGain;

        public:
            explicit Bypass();
            ~Bypass();

        public:
            /**
             * Initialize bypass
             * @param sample_rate sample rate
             * @param time the bypass switch time, by default 5 milliseconds
             */
            void init(int sample_rate, float time = 0.005 );

            /**
             * Process the signal. If Bypass is on, then dry signal is passed to output.
             * If bypass is off, then wet signal is passed.
             * When bypass is in active state, the mix of dry and wet signal is passed to
             * output.
             *
             * @param dst output buffer
             * @param dry dry signal buffer
             * @param wet wet signal buffer
             * @param count number of samples to process
             */
            void process(float *dst, const float *dry, const float *wet, size_t count);

            /**
             * Enable/disable bypass
             * @param bypass bypass value
             * @return true if bypass state has changed
             */
            bool set_bypass(bool bypass);

            /** Enable/disable bypass
             *
             * @param bypass bypass value, when less than 0.5, bypass is considered to become shut down
             * @return true if bypass state has changed
             */
            inline bool set_bypass(float bypass) { return set_bypass(bypass >= 0.5f); };

            /**
             * Return true if bypass is on (final state)
             * @return true if bypass is on
             */
            inline bool on() const      { return nState == S_ON; };

            /**
             * Return true if bypass is off (final state)
             * @return true if bypass is off
             */
            inline bool off() const     { return nState == S_OFF; };

            /**
             * Return true if bypass is active
             * @return true if bypass is active
             */
            inline bool active() const  { return nState == S_ACTIVE; };

            /**
             * Return true if bypass is on or is currently going to become on
             * @return true if bypass is on or is currently going to become on
             */
            bool bypassing() const;
    };

} /* namespace lsp */

#endif /* CORE_UTIL_BYPASS_H_ */
