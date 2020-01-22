/*
 * Crossfade.h
 *
 *  Created on: 22 янв. 2020 г.
 *      Author: sadko
 */

#ifndef CORE_UTIL_CROSSFADE_H_
#define CORE_UTIL_CROSSFADE_H_

#include <core/types.h>

namespace lsp
{
    
    class Crossfade
    {
        private:
            Crossfade & operator = (const Crossfade &);

        protected:
            size_t      nSamples;
            size_t      nCounter;
            float       fDelta;
            float       fGain;

        public:
            explicit Crossfade();
            virtual ~Crossfade();

        public:
            /**
             * Initialize crossfade
             * @param sample_rate sample rate of the signal
             * @param time crossfade time, by default 5 msec
             */
            void init(int sample_rate, float time = 0.005);

            /**
             * Crossfade the signal
             * @param dst destination buffer
             * @param fade_out the signal that will fade out, may be NULL
             * @param fade_in the signal that will fade in, may be NULL
             * @param count number of samples to process
             */
            void process(float *dst, const float *fade_out, const float *fade_in, size_t count);

            /**
             * Return the remaining number of samples to process
             * @return the remaining number of samples to process before crossfade becomes inactive
             */
            inline size_t remaining() const { return nCounter; }

            /**
             * Check if crossfade is currently active
             * @return true if crossfade is currently active
             */
            inline bool active() const { return nCounter > 0; }

            /**
             * Reset the crossfade state, immediately interrupt it's processing
             */
            void reset();

            /**
             * Toggle crossfade processing
             * @return true if crossfade has been toggled, false
             * if crossfade is currently active
             */
            bool toggle();
    };

} /* namespace lsp */

#endif /* CORE_UTIL_CROSSFADE_H_ */
