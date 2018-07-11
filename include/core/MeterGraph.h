/*
 * Meter.h
 *
 *  Created on: 20 мая 2016 г.
 *      Author: sadko
 */

#ifndef CORE_METER_GRAPH_H_
#define CORE_METER_GRAPH_H_

#include <core/types.h>
#include <core/ShiftBuffer.h>

namespace lsp
{
    class MeterGraph
    {
        private:
            ShiftBuffer         sBuffer;
            float               fCurrent;
            size_t              nCount;
            size_t              nPeriod;

        public:
            MeterGraph();
            ~MeterGraph();

        public:
            bool init(size_t frames, size_t period);
            void destroy();

            inline float *data()    { return sBuffer.head();   }

            /** Process single sample
             *
             * @param sample sample to process
             */
            void process(float sample);

            /** Process multiple samples
             *
             * @param s array of samples
             * @param n number of samples to process
             */
            void process(const float *s, size_t n);

            /** Get current level
             *
             * @return current level
             */
            inline float level() const { return sBuffer.last(); }
    };
}

#endif /* CORE_METER_GRAPH_H_ */
