/*
 * Counter.h
 *
 *  Created on: 27 нояб. 2018 г.
 *      Author: sadko
 */

#ifndef INCLUDE_CORE_UTIL_COUNTER_H_
#define INCLUDE_CORE_UTIL_COUNTER_H_

#include <core/types.h>

namespace lsp
{
    class Counter
    {
        protected:
            enum flags_t
            {
                F_INITIAL   = 1 << 0,
                F_FIRED     = 1 << 1
            };

        protected:
            size_t      nCurrent;
            size_t      nInitial;
            size_t      nSampleRate;
            float       fFrequency;
            size_t      nFlags;

        public:
            explicit Counter();
            virtual ~Counter();

        public:
            /**
             * Get sample rate
             * @return sample rate
             */
            inline size_t get_sample_rate() const { return nSampleRate; }

            /**
             * Set sample rate
             * @param sr sample rate
             * @param reset flag to reset counter to initial value
             */
            void set_sample_rate(size_t sr, bool reset = true);

            /**
             * Get frequency
             * @return frequency
             */
            inline float get_frequency() const { return fFrequency; }

            /**
             * Set frequency
             * @param freq frequency
             * @param reset flag to reset counter to initial value
             */
            void set_frequency(float freq, bool reset = true);

            /**
             * Get initial countdown value
             * @return initial countdown value
             */
            inline size_t get_initial_value() const { return nInitial; }

            /**
             * Set initial countdown value
             * @param value initial countdown value
             * @param reset flag to reset counter to initial value
             */
            void set_initial_value(size_t value, bool reset = true);

        public:
            /**
             * Check fired flag
             * @return fired flag
             */
            inline bool fired() const { return nFlags & F_FIRED; }

            /**
             * Get number of samples pending for processing
             * @return number of samples pending for processing
             */
            size_t pending() const { return nCurrent; }

            /**
             * Reset fired flag
             * @return fired flag before reset
             */
            bool commit();

            /**
             * Reset counter to initial value
             * @return fired flag
             */
            bool reset();

            /**
             * Submit number of samples been processed
             * @param samples number of samples to submit
             * @return fired flag
             */
            bool submit(size_t samples);

            /**
             * Prefer frequency over initial value when
             * changing sample rate
             */
            inline void preserve_frequency() {
                nFlags &= ~F_INITIAL;
            }

            /**
             * Prefer initial value over frequency when
             * changing sample rate
             */
            inline void preserve_initial_value() {
                nFlags |= F_INITIAL;
            }
    };
} /* namespace lsp */

#endif /* INCLUDE_CORE_UTIL_COUNTER_H_ */
