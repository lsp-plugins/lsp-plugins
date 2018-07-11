/*
 * Blink.h
 *
 *  Created on: 08 апр. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_BLINK_H_
#define CORE_BLINK_H_

#include <core/types.h>

namespace lsp
{
    /** Simple blink counter
     *
     */
    class Blink
    {
        private:
            ssize_t     nCounter;
            ssize_t     nValue;

        public:
            inline Blink()
            {
                nCounter        = 0.0f;
                nValue          = 0.0f;
            }

            inline ~Blink()
            {
                nCounter        = 0.0f;
                nValue          = 0.0f;
            }

        public:
            /** Initialize bling
             *
             * @param sample_rate sample rate
             * @param time activity time
             */
            inline void init(size_t sample_rate, float time = 0.1)
            {
                nCounter        = 0;
                nValue          = seconds_to_samples(sample_rate, time);
            }

            /** Make blinking
             *
             */
            inline void blink()
            {
                nCounter        = nValue;
            }

            /** Process blinking
             *
             * @return activity value
             */
            inline float process(size_t samples)
            {
                float result    = (nCounter > 0) ? 1.0f : 0.0f;
                nCounter       -= samples;
                return result;
            }
    };

} /* namespace lsp */

#endif /* CORE_BLINK_H_ */
