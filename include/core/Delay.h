/*
 * Delay.hpp
 *
 *  Created on: 06 дек. 2015 г.
 *      Author: sadko
 */

#ifndef DSP_DELAY_HPP_
#define DSP_DELAY_HPP_

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

            bool init(size_t max_size);
            void destroy();

            void process(float *dst, const float *src, size_t count);
            void process(float *dst, const float *src, float gain, size_t count);

            void set_delay(size_t delay);
            inline size_t get_delay() const { return nDelay; };
    };

} /* namespace lsp */

#endif /* DSP_DELAY_HPP_ */
