/*
 * Delay.hpp
 *
 *  Created on: 06 дек. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_UTIL_DELAY_HPP_
#define CORE_UTIL_DELAY_HPP_

#include <core/types.h>
#include <core/dsp.h>

namespace lsp
{
    class Delay
    {
        private:
            float      *pBuffer;
            size_t      nHead;
            size_t      nTail;
            size_t      nDelay;
            size_t      nSize;

        public:
            Delay();
            ~Delay();

            /** Initialize delay
             *
             * @param max_size maximum delay in samples
             * @return status of operation
             */
            bool init(size_t max_size);

            /** Destroy delay
             *
             */
            void destroy();

            /** Process data
             *
             * @param dst destination buffer
             * @param src source buffer
             * @param count number of samples to process
             */
            void process(float *dst, const float *src, size_t count);

            /** Process data
             *
             * @param dst destination buffer
             * @param src source buffer
             * @param gain gain to adjust
             * @param count number of samples to process
             */
            void process(float *dst, const float *src, float gain, size_t count);

            /** Process one sample
             *
             * @param src sample to process
             * @return output sample
             */
            float process(float src);

            /** Clear internal delay buffer
             *
             */
            void clear();

            /** Set delay in samples
             *
             * @param delay delay in samples
             */
            void set_delay(size_t delay);

            /** Get delay in samples
             *
             * @return delay in samples
             */
            inline size_t get_delay() const { return nDelay; };
    };

} /* namespace lsp */

#endif /* CORE_UTIL_DELAY_HPP_ */
